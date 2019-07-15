#include "InitialSslHandshakeTaskFactory.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "mt-core/secure/experimental/cpp/EndpointSSL.hpp"

#include "protos/src/protos/agent.pb.h"

#include <fstream>
#include <string>

// openssl utils
extern std::string RSA_Modulus_from_PEM_f(std::string file_path);
extern std::string RSA_Modulus_short_format(std::string modulus);

void InitialSslHandshakeTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  // TOFIX protocol msg only, its content will not be used
  fetch::oef::pb::Agent_Server_ID id_pb;
  ConstCharArrayBuffer buff(data);
        
  IOefAgentTaskFactory::read(id_pb, buff); // TOFIX

  // get ssl key
  ssl_public_key_ = std::dynamic_pointer_cast<EndpointSSL<std::shared_ptr<google::protobuf::Message>>>(this->getEndpoint()->getEndpoint()->getEndpoint())->get_peer_ssl_key_RSA();
  //public_key_ = id_pb.public_key();
  // agent id is the first/last bytes of ssl public key
  public_key_ = RSA_Modulus_short_format(ssl_public_key_); 

  auto iter = white_list_.find(ssl_public_key_);
  if (iter != white_list_.end())
  {
    agents_->add(public_key_, getEndpoint());
    successor(std::make_shared<OefFunctionsTaskFactory>(core_key_, agents_, public_key_, outbounds));
    FETCH_LOG_INFO(LOGGING_NAME, "Agent ", public_key_," ssl authenticated and white listed");
  }
  else
  {
    FETCH_LOG_WARN(LOGGING_NAME, "Agent ", public_key_," ssl authenticated and NOT white listed. Disconnecting ...");
  }

} 
