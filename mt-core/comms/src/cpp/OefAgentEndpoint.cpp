#include "OefAgentEndpoint.hpp"

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageSender.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"
#include "monitoring/src/cpp/lib/Counter.hpp"
#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"
#include "mt-core/karma/src/cpp/IKarmaPolicy.hpp"

static Gauge count("mt-core.network.OefAgentEndpoint");

OefAgentEndpoint::OefAgentEndpoint(std::shared_ptr<ProtoMessageEndpoint> endpoint)
  : EndpointPipe(std::move(endpoint))
{
  count++;
  ident = count.get();
}

void OefAgentEndpoint::close(const std::string &reason)
{
  Counter("mt-core.network.OefAgentEndpoint.closed")++;
  Counter(std::string("mt-core.network.OefAgentEndpoint.closed.") + reason)++;

  socket().close();
}

void OefAgentEndpoint::setState(const std::string &stateName, bool value)
{
  states[stateName] = value;
}

bool OefAgentEndpoint::getState(const std::string &stateName) const
{
  auto entry = states.find(stateName);
  if (entry == states.end())
  {
    return false;
  }
  return entry->second;
}

void OefAgentEndpoint::setup(IKarmaPolicy *karmaPolicy)
{
  // can't do this in the constructor because shared_from_this doesn't work in there.
  std::weak_ptr<OefAgentEndpoint> myself_wp = shared_from_this();

  endpoint->setOnStartHandler([myself_wp, karmaPolicy](){
      FETCH_LOG_INFO(LOGGING_NAME, "KARMA in OefAgentEndpoint");
      if (auto myself_sp = myself_wp.lock())
      {
        auto k = karmaPolicy -> getAccount(myself_sp -> endpoint -> getRemoteId(), "");
        std::swap(k, myself_sp -> karma);
        myself_sp -> karma . perform("login");
        FETCH_LOG_INFO(LOGGING_NAME, "KARMA: account=", myself_sp -> endpoint -> getRemoteId(), "  balance=", myself_sp -> karma.getBalance());
      }
    });

  auto myGroupId = getIdent();

  endpoint->setOnCompleteHandler([myGroupId, myself_wp](ConstCharArrayBuffer buffers){
    if (auto myself_sp = myself_wp.lock())
    {
      myself_sp -> karma . perform("message");
      Task::setThreadGroupId(myGroupId);
      myself_sp -> factory -> processMessage(buffers);
    }
  });

  endpoint->setOnErrorHandler([myGroupId, myself_wp](const boost::system::error_code& ec) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> karma . perform("error.comms");
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
      Taskpool::getDefaultTaskpool() . lock() -> cancelTaskGroup(myGroupId);
    }
  });

  endpoint->setOnEofHandler([myGroupId, myself_wp]() {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> karma . perform("eof");
      myself_sp -> factory -> endpointClosed();
      myself_sp -> factory.reset();
      Taskpool::getDefaultTaskpool() . lock() -> cancelTaskGroup(myGroupId);
    }
  });

  endpoint->setOnProtoErrorHandler([myGroupId, myself_wp](const std::string &message) {
    if (auto myself_sp = myself_wp.lock()) {
      myself_sp -> karma . perform("error.proto");
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
