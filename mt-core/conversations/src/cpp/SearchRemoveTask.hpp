#pragma once

#include "mt-core/conversations/src/cpp/SearchConversationTask.hpp"
#include "protos/src/protos/search_remove.pb.h"
#include "protos/src/protos/search_response.pb.h"
#include "protos/src/protos/agent.pb.h"

class OutboundConversations;
class OutboundConversation;
class OefAgentEndpoint;


class SearchRemoveTask
: public SearchConversationTask<
    fetch::oef::pb::AgentDescription,
    fetch::oef::pb::Server_AgentMessage,
    fetch::oef::pb::Remove,
    SearchRemoveTask>
{
public:
  using StateResult   = StateMachineTask<SearchRemoveTask>::Result;
  using IN_PROTO      = fetch::oef::pb::AgentDescription;
  using OUT_PROTO     = fetch::oef::pb::Server_AgentMessage;
  using REQUEST_PROTO = fetch::oef::pb::Remove;
  using PARENT = SearchConversationTask<
      fetch::oef::pb::AgentDescription,
      fetch::oef::pb::Server_AgentMessage,
      fetch::oef::pb::Update,
      SearchRemoveTask>;

  SearchRemoveTask(std::shared_ptr<IN_PROTO> initiator,
                   std::shared_ptr<OutboundConversations> outbounds,
                   std::shared_ptr<OefAgentEndpoint> endpoint,
                   uint32_t msg_id,
                   std::string core_key,
                   std::string agent_uri,
                   bool remove_row = false
  );
  virtual ~SearchRemoveTask();

  static constexpr char const *LOGGING_NAME = "SearchRemoveTask";

  std::shared_ptr<Task> get_shared() override
  {
    return shared_from_this();
  }

  StateResult handleResponse(void) override;
  std::shared_ptr<REQUEST_PROTO> make_request_proto() override;

private:
  bool remove_row_;

private:
  SearchRemoveTask(const SearchRemoveTask &other) = delete; // { copy(other); }
  SearchRemoveTask &operator=(const SearchRemoveTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const SearchRemoveTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const SearchRemoveTask &other) = delete; // const { return compare(other)==-1; }
};
