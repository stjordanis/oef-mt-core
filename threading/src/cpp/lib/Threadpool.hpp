#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/ExitState.hpp"

class Threadpool
{
public:
  using ThreadP  = std::shared_ptr<std::thread>;
  using Threads = std::vector<ThreadP>;
  using Mutex = std::mutex;
  using Lock = std::unique_lock<Mutex>;

  using TaskP = std::shared_ptr<Task>;
  using Tasks = std::list<TaskP>;
  using TaskDone = std::pair<ExitState, TaskP>;
  using FinishedTasks = std::list<TaskDone>;
  using RunningTasks = std::map<std::size_t, TaskP>;

  class Task
  {
  public:
    virtual bool isRunnable(void) const;
    virtual ExitState run(void) const;
    virtual void cancel(void) = 0; // try and cancel running task.
  };

  Threadpool():quit(false)
  {
  }
  virtual ~Threadpool()
  {
  }

  void start(std::size_t threadcount)
  {
    threads.reserve(threadcount);
    for (std::size_t thread_idx = 0; thread_idx < threadcount; ++thread_idx)
    {
      threads.emplace_back(
        std::make_shared<std::thread>([this, thread_idx]() { this->runloop(thread_idx); }));
    }
  }

  virtual FinishedTasks getFinishedTasks()
  {
    Lock lock(mutex);
    FinishedTasks result;
    result.swap(finished_tasks);
    return result;
  }

  virtual void shutdown()
  {
    pending_tasks.clear();
    finished_tasks.clear();

    for(auto const &kv : running_tasks)
    {
      kv.second -> cancel();
    }

    quit = true;
    work_available.notify_all();
    work_available.notify_all();
    work_available.notify_all();

    for (auto &thread : threads)
    {
      if (std::this_thread::get_id() != thread->get_id())
      {
        thread->join();
      }
    }
  }

  void post(TaskP task)
  {
    Lock lock(mutex);
    pending_tasks.push_back(task);
    work_available.notify_one();
  }
protected:

  // Threads do this:
  void runloop(std::size_t thread_idx)
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
          pending_tasks.push_back(mytask);
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
          Lock lock(mutex);
          finished_tasks.push_back(TaskDone(status, mytask));
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

private:
  Mutex mutex;
  std::atomic<bool> quit;
  std::condition_variable work_available;
  Tasks pending_tasks;
  FinishedTasks finished_tasks;
  RunningTasks running_tasks;

  Threads threads;

  Threadpool(const Threadpool &other) = delete;
  Threadpool &operator=(const Threadpool &other) = delete;
  bool operator==(const Threadpool &other) = delete;
  bool operator<(const Threadpool &other) = delete;
};
