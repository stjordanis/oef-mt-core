#include "Taskpool.hpp"

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

    {
      Lock lock(mutex);
      if (pending_tasks.empty())
      {
        work_available.wait(lock);
      }
    }

    if (quit)
    {
      return;
    }

    TaskP mytask;
    {
      Lock lock(mutex);
      if (pending_tasks.empty())
      {
        continue; // back to sleep
      }

      mytask = pending_tasks.front();
      pending_tasks.pop_front();
    }

    ExitState status;

    running_tasks[thread_idx] = mytask;

    try
    {
      status = mytask -> run();
    }
    catch(std::exception ex)
    {
      std::cerr << "Threadpool caught:" << ex.what() << std::endl;
      status = ERRORED;
    }
    catch(...)
    {
      std::cerr << "Threadpool caught: other exception" << std::endl;
      status = ERRORED;
    }

    running_tasks[thread_idx].reset();

    switch(status)
    {
    case DEFER:
      //std::cerr << "TASK " << mytask.get() << " DEFER" << std::endl;
      {
        Lock lock(mutex);
        suspended_tasks.insert(mytask);
      }
      break;
    case ERRORED:
      {
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
      }
      //std::cerr << "TASK " << mytask.get() << " ERRORED" << std::endl;
      break;
    case CANCELLED:
      {
        Lock lock(mutex);
        finished_tasks.push_back(TaskDone(status, mytask));
      }
      //std::cerr << "TASK " << mytask.get() << " CANCELLED" << std::endl;
      break;
    case COMPLETE:
      {
        if (autoReapFinishedTasks)
        {
          Lock lock(mutex);
          finished_tasks.push_back(TaskDone(status, mytask));
        }
      }
      //std::cerr << "TASK " << mytask.get() << " COMPLETE" << std::endl;
      break;
    }

    {
      Lock lock(mutex);
      std::cerr << "DONE " << finished_tasks.size() << " TASKS" << std::endl;
    }
  }
}

void Taskpool::remove(TaskP task)
{
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

void Taskpool::submit(TaskP task)
{
  Lock lock(mutex);
  pending_tasks.push_back(task);
  work_available.notify_one();
}

Taskpool::FinishedTasks Taskpool::getFinishedTasks()
{
  Lock lock(mutex);
  FinishedTasks result;
  result.swap(finished_tasks);
  return result;
}
