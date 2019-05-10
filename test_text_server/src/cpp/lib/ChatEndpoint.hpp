#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

class EndpointCollection;

class ChatEndpoint: public Endpoint
{
public:
  std::size_t id;
  std::shared_ptr<TextLineMessageSender> textLineMessageSender;
  
  ChatEndpoint(std::shared_ptr<EndpointCollection> ec, std::size_t id, Core &core);

  virtual ~ChatEndpoint()
  {
  }

  void kill()
  {
    if (onKill)
    {
      onKill();
    }
  }

  ~EndpointCollection()
  {
    Eps local_eps;
    local_eps.swap(eps);
    for(auto& kv : local_eps)
    {
      kv.second -> close();
    }

    while(true)
    {
      int count = 0;
      {
        Lock lock(mutex);
        count = eps.size();
      }

    if (count == 0)
    {
      break;
    }

    sleep(1);
  }
}

  std::shared_ptr<ISocketOwner> createNewConnection(Core &core)
  {
    Lock lock(mutex);
    counter+=1;

    auto ident = counter;

    auto endpoint = std::make_shared<ChatEndpoint>(shared_from_this(), ident, core);

    endpoint -> onError = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
    endpoint -> onEof   = [this, ident](){ this->Eof(ident); };
    endpoint -> onStart = [this, ident, endpoint](){
      this->Start(ident, endpoint);
    };
    return endpoint;
  }
};
