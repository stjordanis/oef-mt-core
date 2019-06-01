#pragma once

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/ExitState.hpp"

template<class SUBCLASS>
class StateMachineTask : public Task
{
public:
  using Result = std::pair<int, ExitState>;
  using EntryPoint = Result (SUBCLASS::*)(void);

  StateMachineTask(SUBCLASS *ptr, const EntryPoint *const entrypoints)
    : Task()
    , entrypoints(entrypoints)
  {
    this -> ptr = ptr;
    this -> state = entrypoints[0];
    runnable = true;
  }
  virtual ~StateMachineTask()
  {
  }

  EntryPoint const *entrypoints;
  static constexpr char const *LOGGING_NAME = "StateMachineTask";

  virtual bool isRunnable(void) const { return runnable; }

  virtual ExitState run(void)
  {
    while(true)
    {
      Result result = (ptr ->* state)();
      
      state = entrypoints[result.first];
      switch(result.second)
      {

      case COMPLETE:
        if (state == 0)
          return COMPLETE;
        break;

      case DEFER:
        if (state == 0)
          return COMPLETE;
        return DEFER;

      case CANCELLED:
      case ERRORED:
        return result.second;
      }
    }
  }

protected:
  EntryPoint state;
  std::atomic<bool> runnable;
  SUBCLASS *ptr;
private:
  StateMachineTask(const StateMachineTask &other) = delete; // { copy(other); }
  StateMachineTask &operator=(const StateMachineTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const StateMachineTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const StateMachineTask &other) = delete; // const { return compare(other)==-1; }
};
