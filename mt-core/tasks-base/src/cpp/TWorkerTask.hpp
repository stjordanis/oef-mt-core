#pragma once

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "threading/src/cpp/lib/Waitable.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include <queue>
#include <memory>
#include <utility>
#include <mutex>
#include <algorithm>
#include <iostream>

template<class WORKLOAD>
class TWorkerTask : public Task
{
public:
  using Workload = WORKLOAD;
  using WorkloadP = std::shared_ptr<Workload>;
  using WaitableP = std::shared_ptr<Waitable>;
  using QueueEntry = std::pair<WorkloadP, WaitableP>;
  using Queue = std::queue<QueueEntry>;

  static constexpr char const *LOGGING_NAME = "TWorkerTask";

  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  using WorkloadProcessed = enum {
    COMPLETE,
    NOT_COMPLETE,
    NOT_STARTED,
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
    WaitableP waitable = std::make_shared<Waitable>();
    QueueEntry qe(workload, waitable);
    queue.push(qe);

    // now make this task runnable.  

    this -> makeRunnable();

    return waitable -> makeNotification();
  }

  virtual WorkloadProcessed process(WorkloadP workload, WorkloadState state) = 0;

  virtual bool isRunnable(void) const
  {
    Lock lock(mutex);
    return !queue.empty();
  }

  bool hasCurrentTask(void) const
  {
    Lock lock(mutex);
    return bool(current.first);
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
          FETCH_LOG_INFO(LOGGING_NAME, "No work, TWorkerTask sleeps");
          return DEFER;
        }
          FETCH_LOG_INFO(LOGGING_NAME, "TWorkerTask gets from queue");
        state = START;
        std::swap(current.first, queue.front().first);
        current.second.swap(queue.front().second);
        queue.pop();
      }
      else
      {
        if (last_result == NOT_STARTED)
        {
          state = START;
        }
        state = RESUME;
      }

      FETCH_LOG_INFO(LOGGING_NAME, "working...");
      auto result = process(current.first, state);
      last_result = result;
      FETCH_LOG_INFO(LOGGING_NAME, "Reply was ", exitStateNames[result]);

      switch(result)
      {
      case COMPLETE:
        {
          Lock lock(mutex);
          current.second -> wake();
          current.first.reset();
        }
        break;
      case NOT_COMPLETE:
        return DEFER;
      case NOT_STARTED:
        return DEFER;
      }
    }
  }

protected:
  QueueEntry current;
  WorkloadProcessed last_result;

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
