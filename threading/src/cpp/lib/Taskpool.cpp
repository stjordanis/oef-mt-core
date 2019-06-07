#include "Taskpool.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "monitoring/src/cpp/lib/Counter.hpp"

static std::weak_ptr<Taskpool> gDefaultTaskPool;

Taskpool::Taskpool(bool autoReapFinishedTasks):quit(false)
{
  this -> autoReapFinishedTasks = autoReapFinishedTasks;
}

void Taskpool::setDefault()
{
  gDefaultTaskPool = shared_from_this();
}

Taskpool::~Taskpool()
{
}

std::weak_ptr<Taskpool> Taskpool::getDefaultTaskpool()
{
  return gDefaultTaskPool;
}

void Taskpool::run(std::size_t thread_idx)
{
  while(true)
  {
    if (quit)
    {
      return;
    }

    auto cv_status = std::cv_status::no_timeout;

    Timestamp now = Clock::now();
    {
      Lock lock(mutex);
      if (pending_tasks.empty())
      {
        cv_status = work_available.wait_until(lock, lockless_getNextWakeTime(now, Milliseconds(100)));
      }
    }

    if (quit)
    {
      return;
    }

    TaskP mytask;

    now = Clock::now();

    {
      Lock lock(mutex);
      mytask = lockless_getNextFutureWork(now);
    }

    if (!mytask)
    {
      Lock lock(mutex);
      if (!pending_tasks.empty())
      {
        mytask = pending_tasks.front();
        mytask -> pool = 0;
        pending_tasks.pop_front();
      }
    }

    if (!mytask)
    {
      continue;
    }

    ExitState status;

    {
      Lock lock(mutex);
      running_tasks[thread_idx] = mytask;
    }

    try
    {
      status = mytask -> run();
    }
    catch(std::exception &ex)
    {
      std::cerr << "Threadpool caught:" << ex.what() << std::endl;
      status = ERRORED;
    }
    catch(...)
    {
      std::cerr << "Threadpool caught: other exception" << std::endl;
      status = ERRORED;
    }

    running_tasks.erase(thread_idx);

    switch(status)
    {
    case DEFER:
      //std::cerr << "TASK " << mytask.get() << " DEFER" << std::endl;
      {
        Lock lock(mutex);
        suspended_tasks.insert(mytask);
        Counter("mt-core.tasks.deferred")++;
      }
      break;
    case ERRORED:
      {
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
        Counter("mt-core.tasks.errored")++;
      }
      //std::cerr << "TASK " << mytask.get() << " ERRORED" << std::endl;
      break;
    case CANCELLED:
      {
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
        Counter("mt-core.tasks.cancelled")++;
      }
      //std::cerr << "TASK " << mytask.get() << " CANCELLED" << std::endl;
      break;
    case COMPLETE:
      {
        if (!autoReapFinishedTasks)
        {
          Lock lock(mutex);
          finished_tasks.push_back(TaskDone(status, mytask));
          Counter("mt-core.tasks.completed")++;
        }
      }
      //std::cerr << "TASK " << mytask.get() << " COMPLETE" << std::endl;
      break;
    }
  }
}

void Taskpool::remove(TaskP task)
{
  Lock lock(mutex);
  auto iter = pending_tasks.begin();
  while(iter != pending_tasks.end())
  {
    if (*iter == task)
    {
      iter = pending_tasks.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

void Taskpool::makeRunnable(TaskP task)
{
  Lock lock(mutex);

  auto iter = suspended_tasks.find(task);
  if (iter != suspended_tasks.end())
  {
    auto task = *iter;
    suspended_tasks.erase(iter);
    pending_tasks.push_front(task);
    work_available.notify_one();
  }
}

Taskpool::TaskpoolStatus Taskpool::getStatus() const
{
  TaskpoolStatus r;
  Lock lock(mutex);

  r.pending_tasks  = pending_tasks.size();
  r.running_tasks   = running_tasks.size();
  r.suspended_tasks = suspended_tasks.size();
  r.future_tasks    = future_tasks.size();
  return r;
}

void Taskpool::stop(void)
{
  quit = true;

  finished_tasks.clear();

  for(auto const &t : pending_tasks)
  {
    t -> cancel();
  }
  pending_tasks.clear();

  for(auto const &kv : running_tasks)
  {
    auto t = kv.second;
    if (t)
    {
      t -> cancel();
    }
  }

  work_available.notify_all();
  work_available.notify_all();
  work_available.notify_all();
}

void Taskpool::suspend(TaskP task)
{
  suspended_tasks.insert(task);
}

void Taskpool::submit(TaskP task)
{
  if (task -> isRunnable())
  {
    Lock lock(mutex);
    pending_tasks.push_back(task);
    work_available.notify_one();
  }
  else
  {
    suspend(task);
  }
}

void Taskpool::after(TaskP task, const Milliseconds &delay)
{
  Lock lock(mutex);
  FETCH_LOG_INFO(LOGGING_NAME, "POSTING AFTER", delay.count());

  std::cout << "POSTING AFTER " << delay.count() << std::endl;

  FutureTask ft;
  ft.task = task;
  ft.due = Clock::now() + delay;

  future_tasks.push(ft);
}

Taskpool::FinishedTasks Taskpool::getFinishedTasks()
{
  Lock lock(mutex);
  FinishedTasks result;
  result.swap(finished_tasks);
  return result;
}

Taskpool::Timestamp Taskpool::lockless_getNextWakeTime(const Timestamp &current_time,
                                                       const Milliseconds &deflt)
{
  Timestamp result = current_time + deflt;
  if (!future_tasks.empty())
  {
    result = future_tasks.top().due;
  }
  return result;
}


Taskpool::TaskP Taskpool::lockless_getNextFutureWork(const Timestamp &current_time)
{
  TaskP result;
  if (!future_tasks.empty())
  {
    if (future_tasks.top().due < current_time)
    {
      result = future_tasks.top().task;
      future_tasks.pop();
    }
  }
  return result;
}


