#pragma once

#include <vector>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"
#include "threading/src/cpp/lib/Threadpool.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "protos/src/protos/config.pb.h"

class OefListenerSet;
class Core;
class IKarmaPolicy;


class MtCore
{
public:
  static constexpr char const *LOGGING_NAME = "MtCore";

  MtCore()
  {
  }
  virtual ~MtCore()
  {
  }

  bool configure(const std::string &config_file="", const std::string &config_json="");
  int run();
protected:
private:
  std::shared_ptr<IKarmaPolicy> karma_policy;
  std::shared_ptr<OefListenerSet> listeners;
  std::shared_ptr<Core> core;
  std::shared_ptr<Taskpool> tasks;
  std::shared_ptr<OutboundConversations> outbounds;
  std::shared_ptr<Agents> agents_;
  fetch::oef::pb::CoreConfig config_;

  Threadpool comms_runners;
  Threadpool tasks_runners;

  void startListeners();
  bool configureFromJsonFile(const std::string &config_file);
  bool configureFromJson(const std::string &config_json);


  MtCore(const MtCore &other) = delete;
  MtCore &operator=(const MtCore &other) = delete;
  bool operator==(const MtCore &other) = delete;
  bool operator<(const MtCore &other) = delete;
};
