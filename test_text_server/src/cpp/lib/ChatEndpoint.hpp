#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

class ChatEndpoint: public Endpoint
{
public:
  std::size_t id;
  std::shared_ptr<TextLineMessageSender> textLineMessageSender;
  
  ChatEndpoint(std::size_t id, Core &core):Endpoint(core, 2000, 2000)
  {
    this -> id = id;
  }

  virtual ~ChatEndpoint()
  {
  }
};
