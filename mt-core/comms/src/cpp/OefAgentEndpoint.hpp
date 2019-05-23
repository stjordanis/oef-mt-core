#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include <memory>

class Core;

class OefAgentEndpoint : public Endpoint
{
public:
  OefAgentEndpoint(Core &core);
  virtual ~OefAgentEndpoint()
  {
  }

protected:
private:
  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
