#pragma once

#include <memory>

#include "threading/src/cpp/lib/ExitState.hpp"

class Taskpool;

class Task: public std::enable_shared_from_this<Task>
{
public:
  virtual bool isRunnable(void) const =0;
  virtual ExitState run(void)=0;
  virtual void cancel(void) // try and cancel running task.
  {
    cancelled = true;
  }

  Task():cancelled(false)
  {
  }

  virtual ~Task()
  {
  }

  bool submit(std::shared_ptr<Taskpool> pool);
  bool submit();

  void makeRunnable();
private:
  std::shared_ptr<Taskpool> pool;
  std::atomic<bool> cancelled;
};
