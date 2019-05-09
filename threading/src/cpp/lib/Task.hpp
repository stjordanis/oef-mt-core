#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"

class Task
{
public:
  virtual bool isRunnable(void) const =0;
  virtual ExitState run(void) const =0;
  virtual void cancel(void) = 0; // try and cancel running task.

  virtual ~Task()
  {
  }
};
