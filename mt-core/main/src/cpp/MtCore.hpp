#pragma once

#include <vector>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/main/src/cpp/MtCoreArgs.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"
#include "threading/src/cpp/lib/Threadpool.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"

class OefListenerSet;
class Core;
class IKarmaPolicy;


class MtCore
{
public:
  using args = MtCoreArgs;

  static constexpr char const *LOGGING_NAME = "MtCore";

  MtCore()
  {
  }
  virtual ~MtCore()
  {
  }

  int run(const args &args);
protected:
private:
  std::shared_ptr<IKarmaPolicy> karma_policy;
  std::shared_ptr<OefListenerSet> listeners;
  std::shared_ptr<Core> core;
  std::shared_ptr<Taskpool> tasks;
  std::shared_ptr<OutboundConversations> outbounds;
  std::shared_ptr<Agents> agents_;

  Threadpool comms_runners;
  Threadpool tasks_runners;

  std::string core_key_;

  void startListeners(const std::vector<uint16_t> &ports);

  MtCore(const MtCore &other) = delete;
  MtCore &operator=(const MtCore &other) = delete;
  bool operator==(const MtCore &other) = delete;
  bool operator<(const MtCore &other) = delete;
};
