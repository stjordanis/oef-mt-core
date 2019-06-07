#include "OefFunctionsTaskFactory.hpp"

#include <stdexcept>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include "protos/src/protos/agent.pb.h"
#include "protos/src/protos/search_update.pb.h"
#include "protos/src/protos/search_remove.pb.h"
#include "protos/src/protos/search_query.pb.h"
#include "protos/src/protos/search_response.pb.h"
#include "mt-core/tasks/src/cpp/AgentToAgentMessageTask.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTask.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTypes.hpp"
#include "mt-core/tasks/src/cpp/TSendProtoTask.hpp"
#include "mt-core/conversations/src/cpp/SearchUpdateTask.hpp"
#include "mt-core/conversations/src/cpp/SearchRemoveTask.hpp"
#include "mt-core/conversations/src/cpp/SearchQueryTask.hpp"


void OefFunctionsTaskFactory::endpointClosed()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Endpoint closed for agent: ", agent_public_key_);
  agents_->remove(agent_public_key_);
}

void OefFunctionsTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  auto envelope = fetch::oef::pb::Envelope();
  IOefAgentTaskFactory::read(envelope, data, data.size - data.current);

  auto payload_case = envelope.payload_case();
  int32_t msg_id = envelope.msg_id();

  OEFURI::URI uri;
  uri.parse(envelope.agent_uri());
  uri.agentKey = agent_public_key_;

  switch(payload_case)
  {
    case fetch::oef::pb::Envelope::kSendMessage:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kSendMessage");
      std::shared_ptr<fetch::oef::pb::Agent_Message> msg_ptr(envelope.release_send_message());
      FETCH_LOG_INFO(LOGGING_NAME, "Got agent message: ", msg_ptr->DebugString());
      auto senderTask = std::make_shared<AgentToAgentMessageTask<fetch::oef::pb::Agent_Message>>(
          agents_->find(agent_public_key_), msg_id, msg_ptr, agents_);
      senderTask->submit();
      break;
    }

    case fetch::oef::pb::Envelope::kRegisterService:
    {
        FETCH_LOG_INFO(LOGGING_NAME, "kRegisterService", envelope.register_service().DebugString());
        auto convTask = std::make_shared<SearchUpdateTask>(
            std::shared_ptr<fetch::oef::pb::AgentDescription>(envelope.release_register_service()),
            outbounds,
            getEndpoint(),
            envelope.msg_id(),
            core_key_,
            uri.agentPartAsString());
        convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kRegisterService REPLY ");
        convTask -> submit();
        break;
    }
    case fetch::oef::pb::Envelope::kUnregisterService:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kUnregisterService", envelope.unregister_service().DebugString());
      auto convTask = std::make_shared<SearchRemoveTask>(
          std::shared_ptr<fetch::oef::pb::AgentDescription>(envelope.release_unregister_service()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.agentPartAsString());
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kUnregisterService REPLY ");
      convTask -> submit();
      break;
    }
    case fetch::oef::pb::Envelope::kRegisterDescription:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kRegisterDescription", envelope.register_description().DebugString());
      auto convTask = std::make_shared<SearchUpdateTask>(
          std::shared_ptr<fetch::oef::pb::AgentDescription>(envelope.release_register_description()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.agentPartAsString());
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kRegisterDescription REPLY ");
      convTask->submit();
      break;
    }
    case fetch::oef::pb::Envelope::kUnregisterDescription:
    {
      //TODO: hack because of nothing
      //envelope.release_unregister_description()
      auto description = std::make_shared<fetch::oef::pb::AgentDescription>();
      FETCH_LOG_INFO(LOGGING_NAME, "kUnregisterDescription", envelope.unregister_description().DebugString());
      auto convTask = std::make_shared<SearchRemoveTask>(
          description,
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.agentPartAsString());
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kUnregisterDescription REPLY ");
      convTask -> submit();
      break;
    }
    case fetch::oef::pb::Envelope::kSearchAgents:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchAgents", envelope.search_agents().DebugString());
      auto convTask = std::make_shared<SearchQueryTask>(
          std::shared_ptr<fetch::oef::pb::AgentSearch>(envelope.release_search_agents()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.toString(), 1);
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kSearchAgents ");
      convTask -> submit();
      break;
    }
    case fetch::oef::pb::Envelope::kSearchServices:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchServices", envelope.search_services().DebugString());
      auto convTask = std::make_shared<SearchQueryTask>(
          std::shared_ptr<fetch::oef::pb::AgentSearch>(envelope.release_search_services()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.toString(), 1);
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kSearchServices ");
      convTask -> submit();
      break;
    }
    case fetch::oef::pb::Envelope::kSearchServicesWide:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchServicesWide", envelope.search_services().DebugString());
      auto convTask = std::make_shared<SearchQueryTask>(
          std::shared_ptr<fetch::oef::pb::AgentSearch>(envelope.release_search_services()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.toString(), 4);
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kSearchServicesWide ");
      convTask -> submit();
    }
    case fetch::oef::pb::Envelope::PAYLOAD_NOT_SET:
      FETCH_LOG_ERROR(LOGGING_NAME, "Cannot process payload ", payload_case, " from ", agent_public_key_);
      break;
  }
}
