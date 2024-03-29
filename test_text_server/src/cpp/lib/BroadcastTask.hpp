#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

template<class ENDPOINT_TYPE, class MESSAGE_TYPE>
class BroadcastTask: public Task
{
public:
  std::shared_ptr<EndpointCollection<ENDPOINT_TYPE>> ec;
  MESSAGE_TYPE s;
  std::size_t from;

  BroadcastTask(
                std::shared_ptr<EndpointCollection<ENDPOINT_TYPE>> ec,
                const MESSAGE_TYPE &s,
                std::size_t from
                )
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

  void broadcast()
  {
    ec -> visit(
                [this](std::shared_ptr<ENDPOINT_TYPE> ep){
                  if (ep -> id != from)
                  {
                    ep -> textLineMessageSender -> send(s);
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

