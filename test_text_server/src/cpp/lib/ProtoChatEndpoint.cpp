#include "test_text_server/src/cpp/lib/ProtoChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

#include "test_text_server/src/cpp/lib/ProtoTextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/ProtoTextLineMessageSender.hpp"

#include "test_text_server/src/cpp/lib/ProtoBroadcastTask.hpp"
#include "test_text_server/src/protos/message.pb.h"

ProtoChatEndpoint::ProtoChatEndpoint(std::shared_ptr<SuitableEndpointCollection> ec, std::size_t id, Core &core):Endpoint(core, 2000, 2000)
{
  auto protoTextLineMessageSenderPtr = std::make_shared<ProtoTextLineMessageSender>();
  auto protoTextLineMessageReaderPtr = std::make_shared<ProtoTextLineMessageReader>();

  //std::cout << "ProtoChatEndpoint::ProtoChatEndpoint READER=" << protoTextLineMessageReaderPtr.get() << std::endl;

  protoTextLineMessageSender = protoTextLineMessageSenderPtr;
  reader = protoTextLineMessageReaderPtr;
  writer = protoTextLineMessageSenderPtr;

  this -> id = id;

  protoTextLineMessageReaderPtr -> onComplete =
    [this, ec](Data s) {
    if (s -> contents() == "shutdown")
    {
      ec -> kill();
    }
    else
    {
      auto task = std::make_shared<ProtoBroadcastTask>(ec, s, this -> id);
      task -> submit();
    }
  };
}

void ProtoChatEndpoint::send(std::shared_ptr<TextLine> &s)
{
  protoTextLineMessageSender -> send(s);
}
