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

  using SELF_P = std::shared_ptr<OefAgentEndpoint>;

  OefAgentEndpoint(std::shared_ptr<ProtoMessageEndpoint> endpoint);
  virtual ~OefAgentEndpoint();

  void setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory);
  void setup(IKarmaPolicy *karmaPolicy);

  virtual void go(void) override
  {
    FETCH_LOG_INFO(LOGGING_NAME, "------------------> OefAgentEndpoint::go");

    SELF_P self = shared_from_this();
    while(!go_functions.empty())
    {
      go_functions.front()(self);
      go_functions.pop_front();
    }

    EndpointPipe<ProtoMessageEndpoint, google::protobuf::Message>::go();
  }

  KarmaAccount karma;

  void close(const std::string &reason);
  void setState(const std::string &stateName, bool value);
  bool getState(const std::string &stateName) const;

  std::size_t getIdent() const
  {
    return ident;
  }

  void addGoFunction(std::function<void(SELF_P)> func)
  {
    go_functions.push_back(func);
  }
protected:
private:
  std::list<std::function<void(SELF_P)>> go_functions;
  std::size_t ident;
  std::shared_ptr<IOefAgentTaskFactory> factory;
  std::map<std::string, bool> states;

  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
