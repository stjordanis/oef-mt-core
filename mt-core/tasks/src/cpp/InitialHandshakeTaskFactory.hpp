#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"

class InitialHandshakeTaskFactory:public IOefAgentTaskFactory
{
public:
  InitialHandshakeTaskFactory(std::shared_ptr<OefAgentEndpoint> the_endpoint):IOefAgentTaskFactory(the_endpoint)
  {
  }
  virtual ~InitialHandshakeTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.
protected:
private:
  InitialHandshakeTaskFactory(const InitialHandshakeTaskFactory &other) = delete;
  InitialHandshakeTaskFactory &operator=(const InitialHandshakeTaskFactory &other) = delete;
  bool operator==(const InitialHandshakeTaskFactory &other) = delete;
  bool operator<(const InitialHandshakeTaskFactory &other) = delete;
};
