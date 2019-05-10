#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"
#include "test_text_server/src/cpp/lib/ChatEndpoint.hpp"

#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"
#include "test_text_server/src/cpp/lib/BroadcastTask.hpp"

EndpointCollection::~EndpointCollection()
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
  //std::cerr << "NUKING..."<<std::endl;
}

std::shared_ptr<ISocketOwner> EndpointCollection::createNewConnection(Core &core)
{
  Lock lock(mutex);
  counter+=1;

  auto ident = counter;

  auto endpoint = std::make_shared<ChatEndpoint>(ident, core);

  auto textLineMessageSenderPtr = std::make_shared<TextLineMessageSender>();
  auto textLineMessageReaderPtr = std::make_shared<TextLineMessageReader>();

  endpoint -> textLineMessageSender = textLineMessageSenderPtr;
  endpoint -> reader = textLineMessageReaderPtr;
  endpoint -> writer = textLineMessageSenderPtr;
  endpoint -> onError = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
  endpoint -> onEof   = [this, ident](){ this->Eof(ident); };
  endpoint -> onStart = [this, ident, endpoint](){
    //std::cerr << "onStart handler" << std::endl;
    this->Start(ident, endpoint);
  };

  textLineMessageReaderPtr -> onComplete =
    [this, textLineMessageSenderPtr, endpoint](const std::string &s) {
    if (s=="shutdown")
    {
      if (onKill)
      {
        onKill();
      }
    }
    auto task = std::make_shared<BroadcastTask>(shared_from_this(), s, endpoint -> id);
    task -> submit();
  };

  return endpoint;
}
