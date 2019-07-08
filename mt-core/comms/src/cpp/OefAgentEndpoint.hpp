#pragma once

#include <memory>
#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/karma/src/cpp/KarmaAccount.hpp"

class Core;
class IOefAgentTaskFactory;
class ProtoMessageReader;
class ProtoMessageSender;
class IKarmaPolicy;

class OefAgentEndpoint
    : public EndpointPipe<ProtoMessageEndpoint, google::protobuf::Message>
    , public std::enable_shared_from_this<OefAgentEndpoint>
{
public:
  static constexpr char const *LOGGING_NAME = "OefAgentEndpoint";

  OefAgentEndpoint(std::shared_ptr<ProtoMessageEndpoint> endpoint);
  virtual ~OefAgentEndpoint();

  void setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory);
  void setup(IKarmaPolicy *karmaPolicy);
protected:
private:
  std::shared_ptr<IOefAgentTaskFactory> factory;
  KarmaAccount karma;

  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
