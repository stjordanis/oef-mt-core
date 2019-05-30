#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"

class InitialHandshakeTaskFactory:public IOefAgentTaskFactory
{
public:
  InitialHandshakeTaskFactory(std::shared_ptr<OefAgentEndpoint> the_endpoint):IOefAgentTaskFactory(the_endpoint)
  {
    state = WAITING_FOR_Agent_Server_ID;
  }
  virtual ~InitialHandshakeTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.

  virtual void endpointClosed(void) {}
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
};
