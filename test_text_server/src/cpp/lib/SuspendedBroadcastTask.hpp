#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

class SuspendedBroadcastTask: public Task
{
public:
  std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec;
  using Data = std::shared_ptr<TextLine>;
  Data s;
  std::size_t from;
  bool state;

  SuspendedBroadcastTask(
                std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec,
                const Data &s,
                std::size_t from
  )
  {
    this -> ec = ec;
    this -> s = s;
    this -> from = from;
    this -> state = false; // not runnable yet.
  }

  virtual ~SuspendedBroadcastTask()
  {
  }

  virtual void makeRunnable(void)
  {
    std::cout << "SuspendedBroadcastTask::makeRunnable" << std::endl;
    state = true;
    Task::makeRunnable();
  }

  virtual bool isRunnable(void) const
  {
    return state;
  }

  void broadcast()
  {
    ec -> visit(
                [this](std::shared_ptr<ProtoChatEndpoint> ep){
                  if (ep -> id != from)
                  {
                    ep -> protoTextLineMessageSender -> send(s);
                    ep -> run_sending();
                  }
                }
                );
  }

  virtual ExitState run(void)
  {
    broadcast();
    return ExitState::COMPLETE;
  }
};

