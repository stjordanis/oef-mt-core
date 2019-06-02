#pragma once

#include <memory>
#include <utility>

#include "threading/src/cpp/lib/StateMachineTask.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTypes.hpp"

class OutboundConversations;
class OutboundConversation;
class OefAgentEndpoint;

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

  SearchConversationTask(SearchConversationType type,
                         std::shared_ptr<google::protobuf::Message> initiator,
                         std::shared_ptr<OutboundConversations> outbounds,
                         std::shared_ptr<OefAgentEndpoint> endpoint
                         );
  virtual ~SearchConversationTask();

  StateResult createConv(void);
  StateResult handleResponse(void);

  static constexpr char const *LOGGING_NAME = "SearchConversationTask";

  std::shared_ptr<google::protobuf::Message> createUpdateApiResponse(std::shared_ptr<google::protobuf::Message> search_response);
  std::shared_ptr<google::protobuf::Message> createRemoveApiResponse(std::shared_ptr<google::protobuf::Message> search_response);
  std::shared_ptr<google::protobuf::Message> createWideSearchApiResponse(std::shared_ptr<google::protobuf::Message> search_response);
  std::shared_ptr<google::protobuf::Message> createSearchApiResponse(std::shared_ptr<google::protobuf::Message> search_response);

  using SendFunc = std::function<void (std::shared_ptr<google::protobuf::Message>, std::shared_ptr<OefAgentEndpoint> endpoint)>;
  SendFunc sendReply;
protected:
  std::shared_ptr<google::protobuf::Message> initiator;
  std::shared_ptr<OutboundConversations> outbounds;
  std::shared_ptr<OutboundConversation> conversation;
  std::shared_ptr<OefAgentEndpoint> endpoint;
  SearchConversationType type;
private:
  SearchConversationTask(const SearchConversationTask &other) = delete; // { copy(other); }
  SearchConversationTask &operator=(const SearchConversationTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const SearchConversationTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const SearchConversationTask &other) = delete; // const { return compare(other)==-1; }
};
