#pragma once

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/ExitState.hpp"

template<class SUBCLASS>
class StateMachineTask : public Task
{
public:
  class StateMachineEntryPointResult;
  using StateMachineEntryPoint =  StateMachineEntryPointResult (SUBCLASS::*)(void); 

  using StateMachineEntryPointResult = std::pair<StateMachineEntryPoint, ExitState>;

  StateMachineTask(StateMachineEntryPoint initialstate) : Task()
  {
    this -> state = initialstate;
    runnable = true;
  }
  virtual ~StateMachineTask()
  {
  }


  virtual bool isRunnable(void) const { return runnable; }

  virtual ExitState run(void)
  {
    while(true)
    {
      auto result = this ->* state();
      state = result.first;
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
  StateMachineEntryPoint state;
  std::atomic<bool> runnable;
private:
  StateMachineTask(const StateMachineTask &other) = delete; // { copy(other); }
  StateMachineTask &operator=(const StateMachineTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const StateMachineTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const StateMachineTask &other) = delete; // const { return compare(other)==-1; }
};
