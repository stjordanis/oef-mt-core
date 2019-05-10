#pragma once

#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <iostream>

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/ExitState.hpp"

class Taskpool: public std::enable_shared_from_this<Taskpool>
{
public:
  using Mutex = std::mutex;
  using Lock = std::unique_lock<Mutex>;

  using TaskP = std::shared_ptr<Task>;
  using Tasks = std::list<TaskP>;
  using TaskDone = std::pair<ExitState, TaskP>;
  using FinishedTasks = std::list<TaskDone>;
  using RunningTasks = std::map<std::size_t, TaskP>;
  using SuspendedTasks = std::set<TaskP>;

  Taskpool(bool autoReapFinishedTasks=true);
  virtual ~Taskpool();

  virtual void submit(TaskP task);
  virtual void run(std::size_t thread_number);
  virtual void setDefault();
  virtual FinishedTasks getFinishedTasks();
  virtual void stop(void);

  static std::weak_ptr<Taskpool> getDefaultTaskpool();

  virtual void remove(TaskP task);
  virtual void makeRunnable(TaskP task);
protected:
private:
  Mutex mutex;
  std::atomic<bool> quit;
  std::condition_variable work_available;
  Tasks pending_tasks;
  FinishedTasks finished_tasks;
  RunningTasks running_tasks;
  SuspendedTasks suspended_tasks;
  bool autoReapFinishedTasks;

  Taskpool(const Taskpool &other) = delete; // { copy(other); }
  Taskpool &operator=(const Taskpool &other) = delete; // { copy(other); return *this; }
  bool operator==(const Taskpool &other) = delete; // const { return compare(other)==0; }
  bool operator<(const Taskpool &other) = delete; // const { return compare(other)==-1; }

};
