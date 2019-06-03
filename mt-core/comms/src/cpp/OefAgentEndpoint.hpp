#pragma once

#include <memory>
#include "mt-core/comms/src/cpp/OefEndpoint.hpp"
#include "fetch_teams/ledger/logger.hpp"

class Core;
class IOefAgentTaskFactory;
class ProtoMessageReader;
class ProtoMessageSender;

class OefAgentEndpoint : public OefEndpoint
{
public:
  static constexpr char const *LOGGING_NAME = "OefAgentEndpoint";


  OefAgentEndpoint(Core &core);
  virtual ~OefAgentEndpoint();

  virtual void go();
  void setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory);
  void setup(std::shared_ptr<OefAgentEndpoint> myself);

protected:
private:
  mutable Mutex mutex;
  std::shared_ptr<IOefAgentTaskFactory> factory;

  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
