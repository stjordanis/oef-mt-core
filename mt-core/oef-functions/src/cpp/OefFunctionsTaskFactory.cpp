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
#include "mt-core/karma/src/cpp/XKarma.hpp"
#include "mt-core/karma/src/cpp/XDisconnect.hpp"
#include "monitoring/src/cpp/lib/Counter.hpp"
#include <random>


static Counter endpoint_closed("mt-core.oef_functions_endpoint_closed");

void OefFunctionsTaskFactory::endpointClosed()
{
  FETCH_LOG_WARN(LOGGING_NAME, "Endpoint closed for agent: ", agent_public_key_, ". Sending removeRow to search...");
  agents_->remove(agent_public_key_);

  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<uint32_t> uniform_dist(1000000, 1000000000);

  endpoint_closed ++;
  auto convTask = std::make_shared<SearchRemoveTask>(
      nullptr,
      outbounds,
      getEndpoint(),
      uniform_dist(e1),
      core_key_,
      agent_public_key_,
      true);
  convTask -> submit();
  convTask -> setGroupId(0);
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

  fetch::oef::pb::Server_AgentMessage_OEFError_Operation operation_code =
    fetch::oef::pb::Server_AgentMessage_OEFError_Operation_OTHER;

  // in case we have to report an error, get the OEFError::Operation
  // code because, sigh, it's different...

  try {

  switch(payload_case)
  {
  default:
    getEndpoint() -> karma . perform("oef.bad.unknown-message");
    break;

    case fetch::oef::pb::Envelope::kSendMessage:
    {
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_SEND_MESSAGE;
      FETCH_LOG_INFO(LOGGING_NAME, "kSendMessage");
      getEndpoint() -> karma . perform("oef.kSendMessage");
      std::shared_ptr<fetch::oef::pb::Agent_Message> msg_ptr(envelope.release_send_message());
      FETCH_LOG_INFO(LOGGING_NAME, "Got agent message: ", msg_ptr->DebugString());
      auto senderTask = std::make_shared<AgentToAgentMessageTask<fetch::oef::pb::Agent_Message>>(
          agents_->find(agent_public_key_), msg_id, msg_ptr, agents_);
      senderTask->submit();
      break;
    }

    case fetch::oef::pb::Envelope::kRegisterService:
    {
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_REGISTER_SERVICE;
      getEndpoint() -> karma . perform("oef.kRegisterService");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_UNREGISTER_SERVICE;
      getEndpoint() -> karma . perform("oef.kUnregisterService");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_REGISTER_DESCRIPTION;
      getEndpoint() -> karma . perform("oef.kRegisterDescription");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_UNREGISTER_DESCRIPTION;
      getEndpoint() -> karma . perform("oef.kUnregisterDescription");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_SEARCH_AGENTS;
      getEndpoint() -> karma . perform("oef.kSearchAgents");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_SEARCH_SERVICES;
      getEndpoint() -> karma . perform("oef.kSearchServices");
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
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_SEARCH_SERVICES_WIDE;
      getEndpoint() -> karma . perform("oef.kSearchServicesWide");
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchServicesWide", envelope.search_services_wide().DebugString());
      auto convTask = std::make_shared<SearchQueryTask>(
          std::shared_ptr<fetch::oef::pb::AgentSearch>(envelope.release_search_services_wide()),
          outbounds,
          getEndpoint(),
          envelope.msg_id(),
          core_key_,
          uri.toString(), 4);
      convTask->setDefaultSendReplyFunc(LOGGING_NAME, "kSearchServicesWide ");
      convTask -> submit();
      break;
    }
    case fetch::oef::pb::Envelope::PAYLOAD_NOT_SET:
      operation_code = fetch::oef::pb::Server_AgentMessage_OEFError_Operation_OTHER;
      getEndpoint() -> karma . perform("oef.bad.nopayload");
      FETCH_LOG_ERROR(LOGGING_NAME, "Cannot process payload ", payload_case, " from ", agent_public_key_);
      break;
  }

  } catch (XKarma &x)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "XKARMA!!!! ", x.what());

    auto error_response = std::make_shared<fetch::oef::pb::Server_AgentMessage>();
    error_response -> set_answer_id(msg_id);
    int failure = operation_code;
    failure |= fetch::oef::pb::Server_AgentMessage_OEFError_Operation_KARMA_;
    error_response -> mutable_oef_error() -> set_operation(fetch::oef::pb::Server_AgentMessage_OEFError_Operation(failure));

    error_response -> mutable_oef_error() -> set_detail(x.what());

    auto senderTask = std::make_shared<TSendProtoTask<fetch::oef::pb::Server_AgentMessage>>(error_response, getEndpoint());
    senderTask -> submit();

  } catch (XDisconnect &x)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "XDISCONNECT was thrown while processing OEF operation: ", x.what());
    throw;
  }
}
