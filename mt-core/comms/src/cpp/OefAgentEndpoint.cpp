#include "OefAgentEndpoint.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"

OefAgentEndpoint::OefAgentEndpoint(Core &core):Endpoint(core, 10000, 10000)
{
  //protoMessageSender = std::make_shared<ProtoTextLineMessageSender>();
  //writer = protoTextLineMessageSender;

  std::cout << "OefAgentEndpoint" << std::endl;

  protoMessageReader = std::make_shared<ProtoMessageReader>();
  reader = protoMessageReader;
}

void OefAgentEndpoint::setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory)
{
  Lock lock(mutex);

  if (factory)
  {
    new_factory -> endpoint = factory -> endpoint;
  }
  factory = new_factory;
}

OefAgentEndpoint::~OefAgentEndpoint()
{
  std::cout << "~OefAgentEndpoint" << std::endl;
}

void OefAgentEndpoint::go(void)
{
  Endpoint::go();
  std::cout << "GO!" << std::endl;
}
