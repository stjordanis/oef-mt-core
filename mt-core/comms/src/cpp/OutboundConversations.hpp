#pragma once

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

#include <string>
#include <memory>

class OutboundConversation;

class OutboundConversations
{
public:
  OutboundConversations();
  virtual ~OutboundConversations();

  std::shared_ptr<OutboundConversation> startConversation(const std::string &target, std::shared_ptr<google::protobuf::Message> initiator);
protected:
private:
  OutboundConversations(const OutboundConversations &other) = delete; // { copy(other); }
  OutboundConversations &operator=(const OutboundConversations &other) = delete; // { copy(other); return *this; }
  bool operator==(const OutboundConversations &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OutboundConversations &other) = delete; // const { return compare(other)==-1; }
};
