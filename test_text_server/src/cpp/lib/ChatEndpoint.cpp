#include "test_text_server/src/cpp/lib/ChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

#include "test_text_server/src/cpp/lib/BroadcastTask.hpp"

ChatEndpoint::ChatEndpoint(std::shared_ptr<SuitableEndpointCollection> ec, std::size_t id, Core &core):Endpoint(core, 2000, 2000)
{
  auto textLineMessageSenderPtr = std::make_shared<TextLineMessageSender>();
  auto textLineMessageReaderPtr = std::make_shared<TextLineMessageReader>();

  textLineMessageSender = textLineMessageSenderPtr;
  reader = textLineMessageReaderPtr;
  writer = textLineMessageSenderPtr;

  this -> id = id;

  textLineMessageReaderPtr -> onComplete =
    [this, ec](const std::string &s) {
    if (s=="shutdown")
    {
      ec -> kill();
    }
    else
    {
      auto task = std::make_shared<BroadcastTask<ChatEndpoint, std::string>>(ec, s, this -> id);
      task -> submit();
    }
  };
}

void ChatEndpoint::send(const std::string &s)
{
  textLineMessageSender -> send(s);
}
