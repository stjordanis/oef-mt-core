#include "OefFunctionsTaskFactory.hpp"

#include <stdexcept>
#include "protos/src/protos/agent.pb.h"

void OefFunctionsTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  auto pb = fetch::oef::pb::Envelope();
  IOefAgentTaskFactory::read(pb, data, data.size - data.current);

  std::cout << "Got an Envelope" << std::endl;
}
