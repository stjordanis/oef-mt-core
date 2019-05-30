#pragma once

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "threading/src/cpp/lib/Waitable.hpp"

#include <queue>
#include <memory>
#include <utility>
#include <mutex>
#include <algorithm>

template<class WORKLOAD>
class TWorkerTask : public Task
{
public:
  using Workload = WORKLOAD;
  using WorkloadP = std::shared_ptr<Workload>;
  using QueueEntry = std::pair<WorkloadP, Waitable>;
  using Queue = std::queue<QueueEntry>;

  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  using WorkloadProcessed = enum {
    COMPLETE,
    NOT_COMPLETE,
  };

  using WorkloadState = enum {
    START,
    RESUME,
  };

  TWorkerTask()
  {
  }
  virtual ~TWorkerTask()
  {
  }

  Notification::NotificationBuilder post(WorkloadP workload)
  {
    Lock lock(mutex);
    queue.push_back(std::make_pair(workload, Waitable()));

    // now make this task runnable.  

    this -> makeRunnable();
  }

  virtual WorkloadProcessed process(WorkloadP workload, WorkloadState state) = 0;

  virtual bool isRunnable(void) const
  {
    Lock lock(mutex);
    return !queue.empty();
  }

  virtual ExitState run(void)
  {
    WorkloadState state;

    while(true)
    {
      if (!current.first)
      {
        Lock lock(mutex);
        if (queue.empty())
        {
          return DEFER;
        }
        state = START;
        std::swap(current.first, queue.front().first);
        current.second.swap(queue.front().second);
        queue.pop();
      }
      else
      {
        state = RESUME;
      }

      auto result = process(current.first, state);
      switch(result)
      {
      case COMPLETE:
        current.second.wake();
        current.first.reset();
        break;
      case NOT_COMPLETE:
        return DEFER;
      }
    }
  }

protected:
  QueueEntry current;

  Queue queue;
  mutable Mutex mutex;
private:
  TWorkerTask(const TWorkerTask &other) = delete;
  TWorkerTask &operator=(const TWorkerTask &other) = delete;
  bool operator==(const TWorkerTask &other) = delete;
  bool operator<(const TWorkerTask &other) = delete;
};

//namespace std { template<> void swap(TWorkerTask& lhs, TWorkerTask& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const TWorkerTask &output) {}
