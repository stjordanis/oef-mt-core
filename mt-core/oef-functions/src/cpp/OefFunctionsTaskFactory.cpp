#include "OefFunctionsTaskFactory.hpp"

#include <stdexcept>
#include "protos/src/protos/agent.pb.h"
#include "protos/src/protos/search_update.pb.h"
#include "mt-core/tasks/src/cpp/AgentToAgentMessageTask.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTask.hpp"
#include "mt-core/conversations/src/cpp/SearchConversationTypes.hpp"
#include "mt-core/tasks/src/cpp/TSendProtoTask.hpp"

void OefFunctionsTaskFactory::endpointClosed()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Endpoint closed for agent: ", agent_public_key_);
  agents_->remove(agent_public_key_);
}

void OefFunctionsTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  auto envelope = fetch::oef::pb::Envelope();
  IOefAgentTaskFactory::read(envelope, data, data.size - data.current);

  FETCH_LOG_INFO(LOGGING_NAME, "2222222222222    Got an Envelope");

  auto payload_case = envelope.payload_case();
  int32_t msg_id = envelope.msg_id();
  switch(payload_case)
  {
    case fetch::oef::pb::Envelope::kSendMessage: {
      FETCH_LOG_INFO(LOGGING_NAME, "kSendMessage");
      std::shared_ptr<fetch::oef::pb::Agent_Message> msg_ptr(envelope.release_send_message());
      FETCH_LOG_INFO(LOGGING_NAME, "Got agent message: ", msg_ptr->DebugString());
      auto senderTask = std::make_shared<AgentToAgentMessageTask<fetch::oef::pb::Agent_Message>>(
          agents_->find(agent_public_key_), msg_id, msg_ptr, agents_);
      senderTask->submit();
      break;
    }

  case fetch::oef::pb::Envelope::kRegisterService:
      FETCH_LOG_INFO(LOGGING_NAME, "kRegisterService");
      //process_register_service(msg_id, envelope.register_service(), envelope.agent_uri());

      {
        auto update = std::make_shared<fetch::oef::pb::Update>();
        update -> set_key("core_name");
        fetch::oef::pb::Update_DataModelInstance* dm = update -> add_data_models();
        dm->set_key(envelope.agent_uri());
        dm->mutable_model()->CopyFrom(envelope.register_service().description().model());
        dm->mutable_values()->CopyFrom(envelope.register_service().description().values());

        auto convTask = std::make_shared<SearchConversationTask>(UPDATE, update, outbounds, getEndpoint());
        (*convTask).sendReply = [](std::shared_ptr<google::protobuf::Message> response, std::shared_ptr<OefAgentEndpoint> endpoint)
          {
            FETCH_LOG_INFO(LOGGING_NAME, "kRegisterService REPLY");
            auto reply_sender = std::make_shared<TSendProtoTask<google::protobuf::Message>>(response, endpoint);
            reply_sender -> submit();
          };
        convTask -> submit();
      }
      break;
    case fetch::oef::pb::Envelope::kUnregisterService:
      FETCH_LOG_INFO(LOGGING_NAME, "kUnregisterService");
      //process_unregister_service(msg_id, envelope.unregister_service(), envelope.agent_uri());
      break;
    case fetch::oef::pb::Envelope::kRegisterDescription:
      FETCH_LOG_INFO(LOGGING_NAME, "kRegisterDescription");
      //process_register_description(msg_id, envelope.register_description());
      break;
    case fetch::oef::pb::Envelope::kUnregisterDescription:
      FETCH_LOG_INFO(LOGGING_NAME, "kUnregisterDescription");
      //process_unregister_description(msg_id);
      break;
    case fetch::oef::pb::Envelope::kSearchAgents:
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchAgents");
      //process_search_agents(msg_id, envelope.search_agents());
      break;
    case fetch::oef::pb::Envelope::kSearchServices:
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchServices");
      //process_search_service(msg_id, envelope.search_services());
      break;
    case fetch::oef::pb::Envelope::kSearchServicesWide:
      FETCH_LOG_INFO(LOGGING_NAME, "kSearchServicesWide");
//       {
//         auto bare_register_service = envelope.search_services_wide();
//              std::shared_ptr<google::protobuf::Message> register_service_message(bare_register_service);
//
//         outbounds -> startConversation(
//                                        Uri("outbound://search/update"),
//                                        register_service_message
//                                        );
//      }
      break;
      //process_search_service_wide(msg_id, envelope.search_services_wide());
      break;
    case fetch::oef::pb::Envelope::PAYLOAD_NOT_SET:
      FETCH_LOG_INFO(LOGGING_NAME, "PAYLOAD_NOT_SET");
      //logger.error("AgentSession::process cannot process payload {} from {}", payload_case, publicKey_);
      break;
  }
}
