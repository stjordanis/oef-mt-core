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
  using Data = std::string;
  using Proto = TextLine;
  using ProtoP = std::shared_ptr<Proto>;

  Data s;

  std::size_t from;
  std::atomic<bool> ready;

  std::list<std::pair<EndpointCollection<ProtoChatEndpoint>::EpIdent, int>> transmissions;

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
        transmissions.push_back(std::make_pair(ident, i));
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
    std::cout << "RUN!!" << transmissions.size() << std::endl;

    auto result = ExitState::COMPLETE;
    while(transmissions.size() > 0)
    {

      if (transmissions.size() < 10)
      {
        return ExitState::COMPLETE;
      }

      auto ident = transmissions.front().first;
      auto numb = transmissions.front().second;
      auto ep = ec -> getEndpoint(ident);
      if (ep)
      {
        auto msg = std::make_shared<Proto>();
        msg -> set_contents(s + "/" + std::to_string(numb));
        if (ep -> protoTextLineMessageSender -> send(msg) . Then([this](){ this -> makeRunnable(); }). Waiting())
        {
          std::cout << "PAUSE!!" << transmissions.size() << std::endl;
          ep -> run_sending();
          result = ExitState::DEFER;
          break;
        }
        ep -> run_sending();
      }
      transmissions.pop_front();
    }
    return result;
  }
};

