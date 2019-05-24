#include "OefFunctionsTaskFactory.hpp"

#include <stdexcept>
#include "protos/src/protos/agent.pb.h"

void OefFunctionsTaskFactory::processMessage(ConstCharArrayBuffer &data)
{
  // Process the message, throw exceptions if they're bad.
  std::cout << "OefFunctionsTaskFactory::processMessage" << std::endl;

  auto pb = fetch::oef::pb::Envelope();
  IOefAgentTaskFactory::read(pb, data, data.size - data.current);

  std::cout << "Got an Envelope" << std::endl;

  //throw std::invalid_argument("Not implemented");
}
