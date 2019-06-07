#include "InitialHandshakeTaskFactory.hpp"

#include "protos/src/protos/agent.pb.h"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "mt-core/oef-functions/src/cpp/OefFunctionsTaskFactory.hpp"
#include "mt-core/tasks/src/cpp/TSendProtoTask.hpp"

void InitialHandshakeTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  fetch::oef::pb::Agent_Server_ID id_pb;
  fetch::oef::pb::Agent_Server_Answer answer_pb;

  ConstCharArrayBuffer buff(data);

  try {
    switch(state)
    {
    case WAITING_FOR_Agent_Server_ID:
      {
        IOefAgentTaskFactory::read(id_pb, buff);
        public_key_ = id_pb.public_key();

        FETCH_LOG_INFO(LOGGING_NAME, "Sending phrase to agent: ", public_key_);
        std::cout << "Sending phrase to agent: " <<  public_key_ << std::endl;

        auto phrase = std::make_shared<fetch::oef::pb::Server_Phrase>();
        phrase -> set_phrase("RandomlyGeneratedString");

        auto senderTask = std::make_shared<TSendProtoTask<fetch::oef::pb::Server_Phrase>>(phrase, getEndpoint());
        senderTask -> submit();
        state = WAITING_FOR_Agent_Server_Answer;
      }
      break;
    case WAITING_FOR_Agent_Server_Answer:
      {
        IOefAgentTaskFactory::read(answer_pb, buff);
        auto connected_pb = std::make_shared<fetch::oef::pb::Server_Connected>();
        connected_pb -> set_status(true);

        FETCH_LOG_INFO(LOGGING_NAME, "Agent ", public_key_, " verified, moving to OefFunctions...");

        auto senderTask = std::make_shared<TSendProtoTask<fetch::oef::pb::Server_Connected>>(connected_pb, getEndpoint());
        senderTask -> submit();
        state = WAITING_FOR_Agent_Server_Answer;

        agents_->add(public_key_, getEndpoint());
        successor(std::make_shared<OefFunctionsTaskFactory>(core_key_, agents_, public_key_, outbounds));
      }
      break;
    }
  }
    catch (std::exception &ex)
    {
      FETCH_LOG_ERROR(LOGGING_NAME, "processMessage  -- ", ex.what());
      throw ex;
    }
  catch (...)
  {
    FETCH_LOG_ERROR(LOGGING_NAME, "processMessage exception");
    throw;
    // ignore the error.
  }
}
