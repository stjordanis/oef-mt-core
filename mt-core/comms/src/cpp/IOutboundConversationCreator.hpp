#pragma once

#include <memory>

class OutboundConversation;

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class IOutboundConversationCreator
{
public:
  IOutboundConversationCreator()
  {
  }
  virtual ~IOutboundConversationCreator()
  {
  }

  virtual std::shared_ptr<OutboundConversation> start(std::shared_ptr<google::protobuf::Message> initiator) = 0;
protected:
private:
  IOutboundConversationCreator(const IOutboundConversationCreator &other) = delete;
  IOutboundConversationCreator &operator=(const IOutboundConversationCreator &other) = delete;
  bool operator==(const IOutboundConversationCreator &other) = delete;
  bool operator<(const IOutboundConversationCreator &other) = delete;
};
