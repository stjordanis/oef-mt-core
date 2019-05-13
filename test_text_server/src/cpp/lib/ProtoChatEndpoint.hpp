#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "test_text_server/src/cpp/lib/ProtoTextLineMessageSender.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

class ProtoChatEndpoint: public Endpoint
{
public:
  using SuitableEndpointCollection = EndpointCollection<ProtoChatEndpoint>;
  using Data = std::shared_ptr<TextLine>;

  std::size_t id;
  std::shared_ptr<ProtoTextLineMessageSender> protoTextLineMessageSender;

  ProtoChatEndpoint(std::shared_ptr<SuitableEndpointCollection> ec, std::size_t id, Core &core);

  virtual ~ProtoChatEndpoint()
  {
  }

  void send(std::shared_ptr<TextLine> &s);
};
