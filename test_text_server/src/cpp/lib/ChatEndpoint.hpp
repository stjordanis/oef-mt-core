#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

class ChatEndpoint: public Endpoint
{
public:
  using SuitableEndpointCollection = EndpointCollection<ChatEndpoint>;

  std::size_t id;
  std::shared_ptr<TextLineMessageSender> textLineMessageSender;

  ChatEndpoint(std::shared_ptr<SuitableEndpointCollection> ec, std::size_t id, Core &core);

  virtual ~ChatEndpoint()
  {
  }

  void send(const std::string &s);
};
