#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"
//#include "fetch_teams/ledger/crypto/ecdsa.hpp"
#include "mt-core/oef-functions/src/cpp/OefFunctionsTaskFactory.hpp"


class InitialSslHandshakeTaskFactory:public IOefAgentTaskFactory
{
public:
  static constexpr char const *LOGGING_NAME = "InitialSslHandshakeTaskFactory";

  InitialSslHandshakeTaskFactory(std::string core_key, std::shared_ptr<OefAgentEndpoint> the_endpoint,
      std::shared_ptr<OutboundConversations> outbounds, std::shared_ptr<Agents> agents)
    : IOefAgentTaskFactory(the_endpoint, outbounds)
  , agents_{std::move(agents)}
  , public_key_{""}
  , core_key_{std::move(core_key)}
  {
  }
  
  virtual ~InitialSslHandshakeTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.

  virtual void endpointClosed(void) {}
protected:
private:

  InitialSslHandshakeTaskFactory(const InitialSslHandshakeTaskFactory &other) = delete;
  InitialSslHandshakeTaskFactory &operator=(const InitialSslHandshakeTaskFactory &other) = delete;
  bool operator==(const InitialSslHandshakeTaskFactory &other) = delete;
  bool operator<(const InitialSslHandshakeTaskFactory &other) = delete;

private:
  std::shared_ptr<Agents> agents_;
  std::string public_key_;
  std::string core_key_;
};
