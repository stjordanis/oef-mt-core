#include "OefAgentEndpoint.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"

static Gauge count("mt-core.network.OefAgentEndpoint");

OefAgentEndpoint::OefAgentEndpoint(std::shared_ptr<ProtoMessageEndpoint> endpoint)
  : EndpointPipe(std::move(endpoint))
{
  count++;
}

void OefAgentEndpoint::setup()
{
  // can't do this in the constructor because shared_from_this doesn't work in there.

  std::weak_ptr<OefAgentEndpoint> myself_wp = shared_from_this();

  auto myGroupId = getIdent();

  endpoint->setOnCompleteHandler([myGroupId, myself_wp](ConstCharArrayBuffer buffers){
    if (auto myself_sp = myself_wp.lock())
    {
      Task::setThreadGroupId(myGroupId);
      myself_sp -> factory -> processMessage(buffers);
    }
  });

  endpoint->setOnErrorHandler([myGroupId, myself_wp](const boost::system::error_code& ec) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
      Taskpool::getDefaultTaskpool() . lock() -> cancelTaskGroup(myGroupId);
    }
  });

  endpoint->setOnEofHandler([myGroupId, myself_wp]() {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
      Taskpool::getDefaultTaskpool() . lock() -> cancelTaskGroup(myGroupId);
    }
  });

  endpoint->setOnProtoErrorHandler([myGroupId, myself_wp](const std::string &message) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
      Taskpool::getDefaultTaskpool() . lock() -> cancelTaskGroup(myGroupId);
    }
  });
}

void OefAgentEndpoint::setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory)
{
  if (factory)
  {
    new_factory -> endpoint = factory -> endpoint;
  }
  factory = new_factory;
}

OefAgentEndpoint::~OefAgentEndpoint()
{
  endpoint->setOnCompleteHandler(nullptr);
  endpoint->setOnErrorHandler(nullptr);
  endpoint->setOnEofHandler(nullptr);
  endpoint->setOnProtoErrorHandler(nullptr);
  FETCH_LOG_INFO(LOGGING_NAME, "~OefAgentEndpoint");
  count--;
}
