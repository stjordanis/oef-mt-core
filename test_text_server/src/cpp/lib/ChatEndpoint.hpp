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

  void send(const std::string &s);
};
