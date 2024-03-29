#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

class EndpointCollection;

class WakeUpTask: public Task
{
public:
  std::shared_ptr<EndpointCollection> ec;
  std::string s;
  std::size_t from;

  WakeUpTask()
  {
    this -> ec = ec;
    this -> s = s;
    this -> from = from;
  }

  virtual ~BroadcastTask()
  {
  }

  virtual bool isRunnable(void) const
  {
    return true;
  }

  void broadcast();

  virtual ExitState run(void)
  {
    broadcast();
    return ExitState::COMPLETE;
  }
};
