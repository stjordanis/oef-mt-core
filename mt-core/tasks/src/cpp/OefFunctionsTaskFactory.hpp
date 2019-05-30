#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "protos/src/protos/agent.pb.h"

class OefFunctionsTaskFactory:public IOefAgentTaskFactory
{
public:

  static constexpr char const *LOGGING_NAME = "OefFunctionsTaskFactory";

  OefFunctionsTaskFactory(std::shared_ptr<Agents> agents, std::string agent_public_key)
  : agents_{std::move(agents)}
  , agent_public_key_{std::move(agent_public_key)}
  {
  }
  virtual ~OefFunctionsTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.

  virtual void endpointClosed(void);
protected:
private:
  OefFunctionsTaskFactory(const OefFunctionsTaskFactory &other) = delete;
  OefFunctionsTaskFactory &operator=(const OefFunctionsTaskFactory &other) = delete;
  bool operator==(const OefFunctionsTaskFactory &other) = delete;
  bool operator<(const OefFunctionsTaskFactory &other) = delete;

private:
  std::shared_ptr<Agents> agents_;
  std::string agent_public_key_;
};
