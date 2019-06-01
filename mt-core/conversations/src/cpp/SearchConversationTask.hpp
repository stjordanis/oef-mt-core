#pragma once

#include <memory>
#include <utility>

#include "threading/src/cpp/lib/StateMachineTask.hpp"

class OutboundConversations;

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class SearchConversationTask
  : public StateMachineTask<SearchConversationTask>
{
public:
  using StateResult = StateMachineTask<SearchConversationTask>::Result;

  SearchConversationTask(std::shared_ptr<google::protobuf::Message> initiator,
                         std::shared_ptr<OutboundConversations> outbounds);
  virtual ~SearchConversationTask();

  StateResult createConv(void);
  StateResult handleResponse(void);

  static constexpr char const *LOGGING_NAME = "SearchConversationTask";

protected:
  std::shared_ptr<google::protobuf::Message> initiator;
  std::shared_ptr<OutboundConversations> outbounds;
private:
  SearchConversationTask(const SearchConversationTask &other) = delete; // { copy(other); }
  SearchConversationTask &operator=(const SearchConversationTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const SearchConversationTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const SearchConversationTask &other) = delete; // const { return compare(other)==-1; }
};
