#pragma once

#include <memory>
#include <utility>

#include "threading/src/cpp/lib/StateMachineTask.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTypes.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/tasks/src/cpp/TSendProtoTask.hpp"
#include "protos/src/protos/agent.pb.h"


template <typename IN_PROTO, typename OUT_PROTO, typename REQUEST_PROTO, typename IMPL_CLASS>
class SearchConverstationTask
    : public StateMachineTask<IMPL_CLASS>
{
public:

  using StateResult = typename StateMachineTask<IMPL_CLASS>::Result;
  using EntryPoint  = typename StateMachineTask<IMPL_CLASS>::EntryPoint;

  static constexpr char const *LOGGING_NAME = "SearchConverstationTask";

  SearchConverstationTask(
      std::string path,
      std::shared_ptr<IN_PROTO> initiator,
      std::shared_ptr<OutboundConversations> outbounds,
      std::shared_ptr<OefAgentEndpoint> endpoint,
      uint32_t msg_id,
      std::string core_key,
      std::string agent_uri,
      const EntryPoint * entryPoints,
      IMPL_CLASS* impl_class_ptr
  )
      :  StateMachineTask<IMPL_CLASS>(impl_class_ptr, entryPoints)
      , initiator(std::move(initiator))
      , outbounds(std::move(outbounds))
      , endpoint(std::move(endpoint))
      , msg_id_(msg_id)
      , core_key_{std::move(core_key)}
      , agent_uri_{std::move(agent_uri)}
      , path_{std::move(path)}
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Task created.");
  }

  virtual ~SearchConverstationTask()
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Task gone.");
  }

  StateResult createConv(void)
  {
    auto this_sp = get_shared();
    std::weak_ptr<Task> this_wp = this_sp;
    FETCH_LOG_INFO(LOGGING_NAME, "Start");
    FETCH_LOG_INFO(LOGGING_NAME, "***PATH: ", path_);
    conversation = outbounds -> startConversation(
        Uri("outbound://search/"+path_),
        make_request_proto()
    );

    if (conversation -> makeNotification().Then( [this_wp](){ auto sp = this_wp.lock(); if (sp) { sp -> makeRunnable(); } } ).Waiting())
    {
      FETCH_LOG_INFO(LOGGING_NAME, "Sleeping");
      return SearchConverstationTask::StateResult(1, DEFER);
    }
    FETCH_LOG_INFO(LOGGING_NAME, "NOT Sleeping");
    return SearchConverstationTask::StateResult(1, COMPLETE);
  }

  virtual StateResult handleResponse(void) = 0;
  virtual std::shared_ptr<Task> get_shared() = 0;
  virtual std::shared_ptr<REQUEST_PROTO> make_request_proto() = 0;

  using SendFunc = std::function<void (std::shared_ptr<OUT_PROTO>, std::shared_ptr<OefAgentEndpoint> endpoint)>;
  SendFunc sendReply;

  void setDefaultSendReplyFunc(const char * logging_name, const char * log_message)
  {
    sendReply = [logging_name, log_message](std::shared_ptr<OUT_PROTO> response, std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      FETCH_LOG_INFO(logging_name, log_message, response->DebugString());
      auto reply_sender = std::make_shared<TSendProtoTask<fetch::oef::pb::Server_AgentMessage>>(response, endpoint);
      reply_sender -> submit();
    };
  }

protected:
  std::shared_ptr<IN_PROTO> initiator;
  std::shared_ptr<OutboundConversations> outbounds;
  std::shared_ptr<OutboundConversation> conversation;
  std::shared_ptr<OefAgentEndpoint> endpoint;
  uint32_t msg_id_;
  std::string core_key_;
  std::string agent_uri_;
  std::string path_;
private:
  SearchConverstationTask(const SearchConverstationTask &other) = delete; // { copy(other); }
  SearchConverstationTask &operator=(const SearchConverstationTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const SearchConverstationTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const SearchConverstationTask &other) = delete; // const { return compare(other)==-1; }
};
