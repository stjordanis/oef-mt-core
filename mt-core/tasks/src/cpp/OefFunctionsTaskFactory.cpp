#include "OefFunctionsTaskFactory.hpp"

#include <stdexcept>
#include "protos/src/protos/agent.pb.h"
#include "mt-core/tasks/src/cpp/AgentToAgentMessageTask.hpp"
#include "fetch_teams/ledger/logger.hpp"


void OefFunctionsTaskFactory::endpointClosed()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Endpoint closed for agent: ", agent_public_key_);
  agents_->remove(agent_public_key_);
}


void OefFunctionsTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  auto envelope = fetch::oef::pb::Envelope();
  IOefAgentTaskFactory::read(envelope, data, data.size - data.current);

  FETCH_LOG_INFO(LOGGING_NAME, "Got an Envelope");

  auto payload_case = envelope.payload_case();
  int32_t msg_id = envelope.msg_id();
  switch(payload_case)
  {
    case fetch::oef::pb::Envelope::kSendMessage: {
      std::shared_ptr<fetch::oef::pb::Agent_Message> msg_ptr(envelope.release_send_message());
      FETCH_LOG_INFO(LOGGING_NAME, "Got agent message: ", msg_ptr->DebugString());
      auto senderTask = std::make_shared<AgentToAgentMessageTask<fetch::oef::pb::Agent_Message>>(
          agents_->find(agent_public_key_), msg_id, msg_ptr, agents_);
      senderTask->submit();
      break;
    }
    case fetch::oef::pb::Envelope::kRegisterService:
      //process_register_service(msg_id, envelope.register_service(), envelope.agent_uri());
      break;
    case fetch::oef::pb::Envelope::kUnregisterService:
      //process_unregister_service(msg_id, envelope.unregister_service(), envelope.agent_uri());
      break;
    case fetch::oef::pb::Envelope::kRegisterDescription:
      //process_register_description(msg_id, envelope.register_description());
      break;
    case fetch::oef::pb::Envelope::kUnregisterDescription:
      //process_unregister_description(msg_id);
      break;
    case fetch::oef::pb::Envelope::kSearchAgents:
      //process_search_agents(msg_id, envelope.search_agents());
      break;
    case fetch::oef::pb::Envelope::kSearchServices:
      //process_search_service(msg_id, envelope.search_services());
      break;
    case fetch::oef::pb::Envelope::kSearchServicesWide:
      //process_search_service_wide(msg_id, envelope.search_services_wide());
      break;
    case fetch::oef::pb::Envelope::PAYLOAD_NOT_SET:
      //logger.error("AgentSession::process cannot process payload {} from {}", payload_case, publicKey_);
      break;
  }
}
