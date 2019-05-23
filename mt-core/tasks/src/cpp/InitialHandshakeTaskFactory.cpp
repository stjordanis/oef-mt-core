#include "InitialHandshakeTaskFactory.hpp"

#include "protos/src/protos/agent.pb.h"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

void InitialHandshakeTaskFactory::processMessage(ConstCharArrayBuffer &data)
{

  int kind = 0;

  fetch::oef::pb::Agent_Server_ID id_pb;
  fetch::oef::pb::Agent_Server_Answer answer_pb;

  try {
    ConstCharArrayBuffer buff(data);
    IOefAgentTaskFactory::read(id_pb, buff);
    kind |= 1;
  }
  catch (...)
  {
    // ignore the error.
  }

  try {
    ConstCharArrayBuffer buff(data);
    IOefAgentTaskFactory::read(answer_pb, buff);
    kind |= 2;
  }
  catch (...)
  {
    // ignore the error.
  }

  switch(kind)
  {
  case 1:
    {
      std::cout << "GOT Agent_Server_ID" << std::endl;
    }
    break;
  case 2:
    {
      std::cout << "GOT Agent_Server_Answer" << std::endl;
    }
    break;
  default:
    getEndpoint() -> close();
    break;
  }
}
