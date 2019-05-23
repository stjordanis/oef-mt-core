#include "IOefAgentTaskFactory.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

void IOefAgentTaskFactory::successor(std::shared_ptr<IOefAgentTaskFactory> factory)
{
  endpoint -> setFactory(factory);
}
