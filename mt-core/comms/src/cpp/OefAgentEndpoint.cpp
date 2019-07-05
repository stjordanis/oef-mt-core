#include "OefAgentEndpoint.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"
#include "mt-core/karma/src/cpp/IKarmaPolicy.hpp"
#include "mt-core/karma/src/cpp/KarmaAccount.hpp"

static Gauge count("mt-core.network.OefAgentEndpoint");

OefAgentEndpoint::OefAgentEndpoint(std::shared_ptr<ProtoMessageEndpoint> endpoint)
  : EndpointPipe(std::move(endpoint))
{
  count++;
  
}

void OefAgentEndpoint::setup(IKarmaPolicy *karmaPolicy)
{
  // can't do this in the constructor because shared_from_this doesn't work in there.

  auto k = karmaPolicy -> getAccount(endpoint -> getRemoteId(), "");
  std::swap(k, karma);

  FETCH_LOG_INFO(LOGGING_NAME, "KARMA: account=", endpoint -> getRemoteId(), "  balance=", karma.getBalance());


  std::weak_ptr<OefAgentEndpoint> myself_wp = shared_from_this();

  endpoint->setOnCompleteHandler([myself_wp](ConstCharArrayBuffer buffers){
    if (auto myself_sp = myself_wp.lock())
    {
      myself_sp -> factory -> processMessage(buffers);
    }
  });

  endpoint->setOnErrorHandler([myself_wp](const boost::system::error_code& ec) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
    }
  });

  endpoint->setOnEofHandler([myself_wp]() {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
    }
  });

  endpoint->setOnProtoErrorHandler([myself_wp](const std::string &message) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
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
