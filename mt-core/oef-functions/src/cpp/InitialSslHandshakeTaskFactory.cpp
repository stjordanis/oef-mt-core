#include "InitialSslHandshakeTaskFactory.hpp"

#include "protos/src/protos/agent.pb.h"

void InitialSslHandshakeTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  fetch::oef::pb::Agent_Server_ID id_pb;
  ConstCharArrayBuffer buff(data);
        
  IOefAgentTaskFactory::read(id_pb, buff);
  public_key_ = id_pb.public_key();
  
  FETCH_LOG_INFO(LOGGING_NAME, "Agent ", public_key_," ssl authenticated");

  agents_->add(public_key_, getEndpoint());
  successor(std::make_shared<OefFunctionsTaskFactory>(core_key_, agents_, public_key_, outbounds));
}
