#include "Taskpool.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "monitoring/src/cpp/lib/Counter.hpp"

static std::weak_ptr<Taskpool> gDefaultTaskPool;

Taskpool::Taskpool(bool autoReapFinishedTasks):quit(false)
{
  this -> autoReapFinishedTasks = autoReapFinishedTasks;

  Counter("mt-core.tasks.popped-for-run");
  Counter("mt-core.tasks.run.std::exception");
  Counter("mt-core.tasks.run.exception");
  Counter("mt-core.tasks.run.deferred");
  Counter("mt-core.tasks.run.errored");
  Counter("mt-core.tasks.run.cancelled");
  Counter("mt-core.tasks.run.completed");
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
        Counter("mt-core.tasks.popped-for-run")++;
        Counter("mt-core.immediate-tasks.popped-for-run")++;
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
      Counter("mt-core.tasks.run.std::exception")++;
      std::cerr << "Threadpool caught:" << ex.what() << std::endl;
      status = ERRORED;
    }
    catch(...)
    {
      Counter("mt-core.tasks.run.exception")++;
      std::cerr << "Threadpool caught: other exception" << std::endl;
      status = ERRORED;
    }

    running_tasks.erase(thread_idx);

    switch(status)
    {
    case DEFER:
      //std::cerr << "TASK " << mytask.get() << " DEFER" << std::endl;
      {
        Counter("mt-core.tasks.run.deferred")++;
        suspend(mytask);
      }
      break;
    case ERRORED:
      {
        Counter("mt-core.tasks.run.errored")++;
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
      }
      //std::cerr << "TASK " << mytask.get() << " ERRORED" << std::endl;
      break;
    case CANCELLED:
      {
        Counter("mt-core.tasks.run.cancelled")++;
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
      }
      //std::cerr << "TASK " << mytask.get() << " CANCELLED" << std::endl;
      break;
    case COMPLETE:
      {
        if (!autoReapFinishedTasks)
        {
          Counter("mt-core.tasks.run.completed")++;
          Lock lock(mutex);
          finished_tasks.push_back(TaskDone(status, mytask));
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
    Counter("mt-core.tasks.made-runnable")++;
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
  Counter("mt-core.tasks.suspended")++;
  task -> pool = shared_from_this();
  suspended_tasks.insert(task);
}

void Taskpool::submit(TaskP task)
{
  if (task -> isRunnable())
  {
    Lock lock(mutex);
    Counter("mt-core.tasks.moved-to-runnable")++;
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
  Counter("mt-core.tasks.futured")++;
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
      result -> pool = 0;
      future_tasks.pop();
      Counter("mt-core.tasks.popped-for-run")++;
      Counter("mt-core.future-tasks.popped-for-run")++;
    }
  }
  return result;
}


