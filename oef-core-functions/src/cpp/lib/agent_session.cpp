//------------------------------------------------------------------------------
//
//   Copyright 2018-2019 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#define DEBUG_ON 1
#include "agent_session.hpp"
#include "utils.hpp"

namespace fetch {
namespace oef {

fetch::oef::Logger AgentSession::logger = fetch::oef::Logger("agent-session");
    
void AgentSession::process_register_description(uint32_t msg_id, const fetch::oef::pb::AgentDescription &desc)
{
  description_ = Instance(desc.description());
  DEBUG_(logger, "AgentSession::processRegisterDescription setting description to agent {} : {}", 
      publicKey_, pbs::to_string(desc));
  
  auto self(shared_from_this()); 
  oef_search_.register_description(*description_, publicKey_, msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processRegisterDescription failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_DESCRIPTION);
        } else {
          DEBUG_(logger, "::processRegisterDescription operation successful for msg {} of agent {}", msg_id, publicKey_);
          // TOFIX should add a status answer, even in the case of no error 
        }
      });
}

void AgentSession::process_unregister_description(uint32_t msg_id) 
{
  if(!description_) {
    DEBUG_(logger, "AgentSession::processUnregisterDescription agent {} hasn't registered description", publicKey_);
    return;// TOFIX should add a status answer, even in the case of no error
  }
  
  auto self(shared_from_this()); 
  oef_search_.unregister_description(*description_, publicKey_, msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processUnregisterDescription failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::UNREGISTER_DESCRIPTION);
        } else {
          DEBUG_(logger, "::processUnregisterDescription operation successful for msg {} of agent {}", msg_id, publicKey_);
          description_ = stde::none; // TOFIX should add a status answer, even in the case of no error
        }
      });
}

void AgentSession::process_register_service(uint32_t msg_id, const fetch::oef::pb::AgentDescription& desc, const std::string& agent_uri)
{
  auto service_desc = Instance(desc.description()); 
  DEBUG_(logger, "AgentSession::processRegisterService registering agent {} ({}) : {}", publicKey_, agent_uri, pbs::to_string(desc));

  OEFURI::URI uri;
  uri.parse(agent_uri);
  uri.agentKey = publicKey_;
  auto self(shared_from_this()); 
  oef_search_.register_service(service_desc, uri.agentPartAsString(), msg_id,
      [this, self, msg_id, agent_uri](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processRegisterService failed operation for msg {} of agent {} ({})", msg_id, publicKey_, agent_uri);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_SERVICE);
        } else {
          DEBUG_(logger, "::processRegisterService operation successful for msg {} of agent {} ({})", msg_id, publicKey_, agent_uri);
          // TOFIX should add a status answer, even in the case of no error 
        }
      });
}

void AgentSession::process_unregister_service(uint32_t msg_id, const fetch::oef::pb::AgentDescription &desc, const std::string& agent_uri)
{
  auto service_desc = Instance(desc.description()); 
  DEBUG_(logger, "AgentSession::processUnregisterService unregistering agent {} : {}", publicKey_, pbs::to_string(desc));

  OEFURI::URI uri;
  uri.parse(agent_uri);
  uri.agentKey = publicKey_;

  auto self(shared_from_this()); 
  oef_search_.unregister_service(service_desc, uri.agentPartAsString(), msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processUnregisterService failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::UNREGISTER_SERVICE);
        } else {
          DEBUG_(logger, "::processUnregisterService operation successful for msg {} of agent {}", msg_id, publicKey_);
          // TOFIX should add a status answer, even in the case of no error 
        }
      });
}

void AgentSession::process_search_agents(uint32_t msg_id, const fetch::oef::pb::AgentSearch &search) 
{
  auto query = QueryModel(search.query());
  DEBUG_(logger, "AgentSession::processSearchAgents from agent {} : {}", publicKey_, pbs::to_string(search));
  
  auto self(shared_from_this()); 
  oef_search_.search_agents(query, publicKey_, msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processSearchAgents failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_SERVICE);
        } else {
          DEBUG_(logger, "::processSearchAgents operation successful for msg {} of agent {}", msg_id, publicKey_);
          fetch::oef::pb::Server_AgentMessage answer;
          answer.set_answer_id(msg_id);
          auto answer_agents = answer.mutable_agents();
          for(auto &a : response.agents) {
            OEFURI::URI uri;
            uri.parseAgent(a);
            answer_agents->add_agents(uri.agentPartAsString());
          }
          logger.trace("AgentSession::processSearchAgents sending {} agents to {}", answer_agents->agents().size(), publicKey_);
          send(answer);
        }
      });
}

void AgentSession::process_search_service(uint32_t msg_id, const fetch::oef::pb::AgentSearch &search) 
{
  auto query = QueryModel(search.query());
  DEBUG_(logger, "AgentSession::processQuery from agent {} : {}", publicKey_, pbs::to_string(search));
  
  auto self(shared_from_this()); 
  oef_search_.search_service(query, publicKey_, msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processQuery failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_SERVICE);
        } else {
          DEBUG_(logger, "::processQuery operation successful for msg {} of agent {}", msg_id, publicKey_);
          fetch::oef::pb::Server_AgentMessage answer;
          answer.set_answer_id(msg_id);
          auto answer_agents = answer.mutable_agents();
          for(auto &a : response.agents) {
            OEFURI::URI uri;
            uri.parseAgent(a);
            answer_agents->add_agents(uri.agentPartAsString());
          }
          logger.trace("AgentSession::processQuery sending {} agents to {}", answer_agents->agents().size(), publicKey_);
          send(answer);
        }
      });
}

void AgentSession::process_search_service_wide(uint32_t msg_id, const fetch::oef::pb::AgentSearch &search) 
{
  auto query = QueryModel(search.query());
  DEBUG_(logger, "AgentSession::processQueryWide from agent {} : {}", publicKey_, pbs::to_string(search));
  
  auto self(shared_from_this()); 
  oef_search_.search_service_wide(query, publicKey_, msg_id,
      [this, self, msg_id](std::error_code ec, OefSearchResponse response) {
        if (ec) {
          DEBUG_(logger, "::processQueryWide failed operation for msg {} of agent {}", msg_id, publicKey_);
          send_error(msg_id, fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_SERVICE);
        } else {
          DEBUG_(logger, "::processQueryWide operation successful for msg {} of agent {}", msg_id, publicKey_);
          fetch::oef::pb::Server_AgentMessage answer;
          answer.set_answer_id(msg_id);
          answer.mutable_agents_wide()->CopyFrom(response.search_result_wide);
          //
          int agents_nbr = 0;
          auto items = response.search_result_wide.result();
          for (auto& item : items) {
            agents_nbr+= item.agents().size();
          }
          logger.trace("AgentSession::processQueryWide sending {} agents to {}", agents_nbr, publicKey_);
          send(answer);
        }
      });
}

void AgentSession::send_dialog_error(uint32_t msg_id, uint32_t dialogue_id, const std::string &origin) {
  fetch::oef::pb::Server_AgentMessage answer;
  answer.set_answer_id(msg_id);
  auto *error = answer.mutable_dialogue_error();
  error->set_dialogue_id(dialogue_id);
  error->set_origin(origin);
  logger.trace("AgentSession::process_message sending dialogue error {} to {}", dialogue_id, publicKey_);
  send(answer);
}

void AgentSession::send_error(uint32_t msg_id, fetch::oef::pb::Server_AgentMessage_OEFError_Operation error_op) {
  fetch::oef::pb::Server_AgentMessage answer;
  answer.set_answer_id(msg_id);
  auto *error = answer.mutable_oef_error();
  error->set_operation(error_op);
  logger.trace("AgentSession:: sending error {} to {}", error->operation(), publicKey_);
  send(answer);
}

void AgentSession::process_message(uint32_t msg_id, fetch::oef::pb::Agent_Message *msg) 
{
  OEFURI::URI uri;
  uri.parseAgent(msg->destination());
  auto session = agentDirectory_.session(uri.agentKey);
  DEBUG_(logger, "AgentSession::process_message from agent {} : {}", publicKey_, pbs::to_string(*msg));
  logger.trace("AgentSession::process_message to {} from {}", msg->destination(), publicKey_);
  uint32_t did = msg->dialogue_id();
  if(session) {
    fetch::oef::pb::Server_AgentMessage message;
    message.set_answer_id(msg_id);
    message.set_source_uri(msg->source_uri());
    message.set_target_uri(msg->target_uri());
    if (message.target_uri().size()==0) {
      message.set_target_uri(uri.toString());
    }
    auto content = message.mutable_content();
    content->set_dialogue_id(did);
    content->set_origin(publicKey_);
    if(msg->has_content()) {
      content->set_allocated_content(msg->release_content());
    }
    if(msg->has_fipa()) {
      content->set_allocated_fipa(msg->release_fipa());
    }
    DEBUG_(logger, "AgentSession::process_message to agent {} : {}", msg->destination(), pbs::to_string(message));
    session->send(message, 
        [this,did,msg_id,msg](std::error_code ec, std::size_t length) {
          if(ec) {
            send_dialog_error(msg_id, did, msg->destination());
          }
        }); 
  } else {
    send_dialog_error(msg_id, did, msg->destination());
  }
}

void AgentSession::process(const std::shared_ptr<Buffer> &buffer) {
  auto envelope = pbs::deserialize<fetch::oef::pb::Envelope>(*buffer);
  auto payload_case = envelope.payload_case();
  uint32_t msg_id = envelope.msg_id();
  switch(payload_case) {
    case fetch::oef::pb::Envelope::kSendMessage:
      process_message(msg_id, envelope.release_send_message());
      break;
    case fetch::oef::pb::Envelope::kRegisterService:
      process_register_service(msg_id, envelope.register_service(), envelope.agent_uri());
      break;
    case fetch::oef::pb::Envelope::kUnregisterService:
      process_unregister_service(msg_id, envelope.unregister_service(), envelope.agent_uri());
      break;
    case fetch::oef::pb::Envelope::kRegisterDescription:
      process_register_description(msg_id, envelope.register_description());
      break;
    case fetch::oef::pb::Envelope::kUnregisterDescription:
      process_unregister_description(msg_id);
      break;
    case fetch::oef::pb::Envelope::kSearchAgents:
      process_search_agents(msg_id, envelope.search_agents());
      break;
    case fetch::oef::pb::Envelope::kSearchServices:
      process_search_service(msg_id, envelope.search_services());
      break;
    case fetch::oef::pb::Envelope::kSearchServicesWide:
      process_search_service_wide(msg_id, envelope.search_services_wide());
      break;
    case fetch::oef::pb::Envelope::PAYLOAD_NOT_SET:
      logger.error("AgentSession::process cannot process payload {} from {}", payload_case, publicKey_);
  }
}
      
void AgentSession::read() {
        auto self(shared_from_this());
        comm_->receive_async([this, self](std::error_code ec, std::shared_ptr<Buffer> buffer) {
                                if(ec) {
                                  agentDirectory_.remove(publicKey_);
                                  logger.info("AgentSession::read error on id {} ec {}", publicKey_, ec);
                                } else {
                                  process(buffer);
                                  read();
                                }
                             });
}

} // oef
} // fetch
