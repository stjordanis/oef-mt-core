#include "InitialHandshakeTaskFactory.hpp"

#include "protos/src/protos/agent.pb.h"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "mt-core/tasks/src/cpp/TSendProtoTask.hpp"

void InitialHandshakeTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  std::cout << "InitialHandshakeTaskFactory::processMessage" << std::endl;

  fetch::oef::pb::Agent_Server_ID id_pb;
  fetch::oef::pb::Agent_Server_Answer answer_pb;

  ConstCharArrayBuffer buff(data);

  try {
    switch(state)
    {
    case WAITING_FOR_Agent_Server_ID:
      {
        IOefAgentTaskFactory::read(id_pb, buff);
        std::cout << "GOT Agent_Server_ID" << std::endl;

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

        std::cout << "SUCCESSFUL ident \"" << answer_pb.answer()<< "\"" << std::endl;

        auto connected_pb = std::make_shared<fetch::oef::pb::Server_Connected>();
        connected_pb -> set_status(true);

        auto senderTask = std::make_shared<TSendProtoTask<fetch::oef::pb::Server_Connected>>(connected_pb, getEndpoint());
        senderTask -> submit();
        state = WAITING_FOR_Agent_Server_Answer;
      }
      break;
    }
  }
  catch (std::exception &ex)
  {
    std::cout << "InitialHandshakeTaskFactory::processMessage  -- " << ex.what() << std::endl;
    // ignore the error.
  }
  catch (...)
  {
    std::cout << "InitialHandshakeTaskFactory::processMessage exception" << std::endl;
    // ignore the error.
  }
}
