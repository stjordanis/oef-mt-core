#include "ProtoMessageEndpoint.hpp"

#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"


ProtoMessageEndpoint::ProtoMessageEndpoint(std::shared_ptr<EndpointType> endpoint)
  : EndpointPipe(std::move(endpoint))
{
}

void ProtoMessageEndpoint::setup(std::shared_ptr<ProtoMessageEndpoint>& myself)
{
  std::weak_ptr<ProtoMessageEndpoint> myself_wp = myself;

  protoMessageSender = std::make_shared<ProtoMessageSender>(myself_wp);
  endpoint->writer = protoMessageSender;

  protoMessageReader = std::make_shared<ProtoMessageReader>(myself_wp);
  endpoint->reader = protoMessageReader;
}


void ProtoMessageEndpoint::setEndianness(Endianness newstate)
{
  protoMessageReader -> setEndianness(newstate);
  protoMessageSender -> setEndianness(newstate);
}

