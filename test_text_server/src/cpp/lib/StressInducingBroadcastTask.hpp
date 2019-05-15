#pragma once

#include "threading/src/cpp/lib/ExitState.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"
#include "threading/src/cpp/lib/Notification.hpp"

class StressInducingBroadcastTask: public Task
{
public:
  std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec;
  using Data = std::shared_ptr<TextLine>;
  Data s;
  std::size_t from;
  Notification waiting;
  std::atomic<bool> ready;

  std::list<EndpointCollection<ProtoChatEndpoint>::EpIdent> transmissions;

  EndpointCollection<ProtoChatEndpoint>::EpIdents endpoint_idents;

  StressInducingBroadcastTask(
                std::shared_ptr<EndpointCollection<ProtoChatEndpoint>> ec,
                Data s,
                std::size_t from
                              ):ready{true}
  {
    this -> ec = ec;
    this -> s = s;
    this -> from = from;

    endpoint_idents = ec->getEndpointIdents();

    for(auto& ident : endpoint_idents)
    {
      if (ident == from)
      {
        continue;
      }
      for(int i=0;i<100;i++)
      {
        transmissions.push_back(ident);
      }
    }
  }

  virtual ~StressInducingBroadcastTask()
  {
  }

  virtual bool isRunnable(void) const
  {
//    if (transmissions.size() == 0)
//    {
      return true;
//    }
//    return ready;
  }

  virtual ExitState run(void)
  {
    std::cout << "RUN!!"<< std::endl;
    //EndpointCollection<ProtoChatEndpoint>::EpIdent cur = -1;
    while(transmissions.size() > 0)
    {
      auto ident = transmissions.front();
      auto ep = ec -> getEndpoint(ident);
      if (ep)
      {
        ep -> protoTextLineMessageSender -> send(s);
        ep -> run_sending();
      }
      transmissions.pop_front();
    }
    return ExitState::COMPLETE;
  }
};

