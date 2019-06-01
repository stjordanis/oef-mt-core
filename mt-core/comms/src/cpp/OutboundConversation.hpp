#pragma once

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

#include <memory>

#include "threading/src/cpp/lib/Waitable.hpp"

class Task;

class OutboundConversation : public Waitable
{
protected:
  friend class OutboundConversations;
  OutboundConversation()
  {
  }
public:
  virtual ~OutboundConversation()
  {
  }

  virtual std::size_t getAvailableReplyCount() const = 0;
  virtual std::shared_ptr<google::protobuf::Message> getReply(std::size_t replynumber) = 0;
protected:
private:
  OutboundConversation(const OutboundConversation &other) = delete; // { copy(other); }
  OutboundConversation &operator=(const OutboundConversation &other) = delete; // { copy(other); return *this; }
  bool operator==(const OutboundConversation &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OutboundConversation &other) = delete; // const { return compare(other)==-1; }
};

//namespace std { template<> void swap(OutboundConversation& lhs, OutboundConversation& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const OutboundConversation &output) {}
