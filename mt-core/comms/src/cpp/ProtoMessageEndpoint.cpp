#include "ProtoMessageEndpoint.hpp"

#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"

ProtoMessageEndpoint::ProtoMessageEndpoint(Core &core)
  : Endpoint(core, 1000000, 1000000)
{
}

// can't do this in the constructor because shared_from_this doesn't work in there.

void ProtoMessageEndpoint::setup(std::shared_ptr<ProtoMessageEndpoint> myself)
{
  std::weak_ptr<ProtoMessageEndpoint> myself_wp = myself;

  protoMessageSender = std::make_shared<ProtoMessageSender>(myself_wp);
  writer = protoMessageSender;

  protoMessageReader = std::make_shared<ProtoMessageReader>(myself_wp);
  reader = protoMessageReader;
}

void ProtoMessageEndpoint::setEndianness(Endianness newstate)
{
  protoMessageReader -> setEndianness(newstate);
  protoMessageSender -> setEndianness(newstate);
}

Notification::NotificationBuilder ProtoMessageEndpoint::send(std::shared_ptr<google::protobuf::Message> s)
{
  return protoMessageSender -> send(s);
}
