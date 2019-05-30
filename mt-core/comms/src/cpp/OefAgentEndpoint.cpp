#include "OefAgentEndpoint.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"

OefAgentEndpoint::OefAgentEndpoint(Core &core):OefEndpoint(core)
{
}

void OefAgentEndpoint::setup(std::shared_ptr<OefAgentEndpoint> myself)
{
  // can't do this in the constructor because shared_from_this doesn't work in there.

  OefEndpoint::setup(myself);

  std::weak_ptr<OefAgentEndpoint> myself_wp = myself;
  auto completionHandler = [myself_wp](ConstCharArrayBuffer buffers){
    if (auto myself_sp = myself_wp.lock())
    {
      myself_sp -> factory -> processMessage(buffers);
    }
  };
  protoMessageReader -> onComplete = completionHandler;

  onError = [this](const boost::system::error_code& ec) { if (factory) { factory -> endpointClosed(); factory.reset(); } };
  onEof = [this]()                                      { if (factory) { factory -> endpointClosed(); factory.reset(); } };
  onProtoError = [this](const std::string &message)     { if (factory) { factory -> endpointClosed(); factory.reset(); } };
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
