#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <iostream>
#include "threading/src/cpp/lib/Task.hpp"

class Threadpool
{
public:
  using TaskP = std::shared_ptr<Task>;
  using Tasks = std::list<TaskP>;
  using ThreadP  = std::shared_ptr<std::thread>;
  using Threads = std::vector<ThreadP>;
  using Mutex = std::mutex;
  using Lock = std::unique_lock<Mutex>;

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

  void start(unsigned int threadcount)
  {
    threads.reserve(threadcount);
    for (std::size_t thread_idx = 0; thread_idx < threadcount; ++thread_idx)
    {
      threads.emplace_back(
          std::make_shared<std::thread>([this]() { this->runloop(); }));
    }
  }

  virtual void shutdown()
  {
    tasks.clear();
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
    tasks.push_back(task);
    work_available.notify_one();
  }
protected:

  // Threads do this:
  void runloop()
  {
    while(true)
    {
      if (quit)
      {
        return;
      }

      {
        Lock lock(mutex);
        if (tasks.empty())
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
        if (tasks.empty())
        {
          continue; // back to sleep
        }

        mytask = tasks.front();
        tasks.pop_front();
      }

      ExitState status;

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

      switch(status)
      {
      case DEFER:
        std::cerr << "TASK " << mytask.get() << " DEFER" << std::endl;
        {
          Lock lock(mutex);
          tasks.push_back(mytask);
        }
        break;
      case ERRORED:
        std::cerr << "TASK " << mytask.get() << " ERRORED" << std::endl;
        break;
      case CANCELLED:
        std::cerr << "TASK " << mytask.get() << " CANCELLED" << std::endl;
        break;
      case COMPLETE:
        std::cerr << "TASK " << mytask.get() << " COMPLETE" << std::endl;
        break;
      }
    }
  }

private:
  Mutex mutex;
  std::atomic<bool> quit;
  std::condition_variable work_available;
  Tasks tasks;

  Threads threads;

  Threadpool(const Threadpool &other) = delete;
  Threadpool &operator=(const Threadpool &other) = delete;
  bool operator==(const Threadpool &other) = delete;
  bool operator<(const Threadpool &other) = delete;
};
