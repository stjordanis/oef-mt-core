#include "test_text_server/src/cpp/lib/ProtoChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

#include "test_text_server/src/cpp/lib/ProtoTextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/ProtoTextLineMessageSender.hpp"

#include "test_text_server/src/cpp/lib/ProtoBroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/DelayedBroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/SuspendedBroadcastTask.hpp"
#include "test_text_server/src/protos/message.pb.h"

ProtoChatEndpoint::ProtoChatEndpoint(std::shared_ptr<SuitableEndpointCollection> ec, std::size_t id, Core &core):Endpoint(core, 2000, 2000)
{
  protoTextLineMessageSender = std::make_shared<ProtoTextLineMessageSender>();
  protoTextLineMessageReader = std::make_shared<ProtoTextLineMessageReader>();
  reader = protoTextLineMessageReader;
  writer = protoTextLineMessageSender;

  this -> id = id;

  protoTextLineMessageReader -> onComplete =
    [this, ec](Data s) {
    if (s -> contents() == "shutdown")
    {
      ec -> kill();
    }
    else
    {
      auto task = std::make_shared<ProtoBroadcastTask>(ec, s, this -> id);
      task -> submit();

      auto later = std::make_shared<TextLine>();
      later -> set_contents(s -> contents() + "(delayed 2s)");

      auto suspended = std::make_shared<TextLine>();
      suspended -> set_contents(s -> contents() + "(suspended)");

      auto task3 = std::make_shared<SuspendedBroadcastTask>(ec, suspended, this -> id);
      task3 -> submit();

      auto task2 = std::make_shared<DelayedBroadcastTask>(ec, later, this -> id, task3);
      task2 -> submit(std::chrono::milliseconds(2000));
    }
  };
}

void ProtoChatEndpoint::send(std::shared_ptr<TextLine> &s)
{
  protoTextLineMessageSender -> send(s);
}
