#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"
#include "test_text_server/src/cpp/lib/SuspendedBroadcastTask.hpp"

class DelayedBroadcastTask: public Task
{
public:
  std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec;
  using Data = std::shared_ptr<TextLine>;
  Data s;
  std::size_t from;
  std::shared_ptr<SuspendedBroadcastTask> wakeup;

  DelayedBroadcastTask(
                std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec,
                const Data &s,
                std::size_t from,
                std::shared_ptr<SuspendedBroadcastTask> wakeup
  )
  {
    this -> ec = ec;
    this -> s = s;
    this -> from = from;
    this -> wakeup = wakeup;
  }

  virtual ~DelayedBroadcastTask()
  {
  }

  virtual bool isRunnable(void) const
  {
    return true;
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
    if (wakeup)
    {
      wakeup -> makeRunnable();
    }
    return ExitState::COMPLETE;
  }
};

