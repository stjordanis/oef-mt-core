#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"


class InitialHandshakeTaskFactory:public IOefAgentTaskFactory
{
public:
  static constexpr char const *LOGGING_NAME = "InitialHandshakeTaskFactory";

  InitialHandshakeTaskFactory(std::shared_ptr<OefAgentEndpoint> the_endpoint, std::shared_ptr<Agents> agents)
  : IOefAgentTaskFactory(the_endpoint)
  , agents_{std::move(agents)}
  , public_key_{""}
  {
    state = WAITING_FOR_Agent_Server_ID;
  }
  virtual ~InitialHandshakeTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.
protected:
private:

  enum {
    WAITING_FOR_Agent_Server_ID,
    WAITING_FOR_Agent_Server_Answer,
  } state;

  InitialHandshakeTaskFactory(const InitialHandshakeTaskFactory &other) = delete;
  InitialHandshakeTaskFactory &operator=(const InitialHandshakeTaskFactory &other) = delete;
  bool operator==(const InitialHandshakeTaskFactory &other) = delete;
  bool operator<(const InitialHandshakeTaskFactory &other) = delete;

private:
  std::shared_ptr<Agents> agents_;
  std::string public_key_;
};
