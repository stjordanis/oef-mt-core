#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"

class OefFunctionsTaskFactory:public IOefAgentTaskFactory
{
public:
  OefFunctionsTaskFactory()
  {
  }
  virtual ~OefFunctionsTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.

  virtual void endpointClosed(void) {}
protected:
private:
  OefFunctionsTaskFactory(const OefFunctionsTaskFactory &other) = delete;
  OefFunctionsTaskFactory &operator=(const OefFunctionsTaskFactory &other) = delete;
  bool operator==(const OefFunctionsTaskFactory &other) = delete;
  bool operator<(const OefFunctionsTaskFactory &other) = delete;
};
