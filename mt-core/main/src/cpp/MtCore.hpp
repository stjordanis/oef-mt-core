#pragma once

#include <vector>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/main/src/cpp/MtCoreArgs.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"
#include "threading/src/cpp/lib/Threadpool.hpp"

class OefListenerSet;
class Core;


class MtCore
{
public:
  using args = MtCoreArgs;

  MtCore()
  {
  }
  virtual ~MtCore()
  {
  }

  int run(const args &args);
protected:
private:
  std::shared_ptr<OefListenerSet> listeners;
  std::shared_ptr<Core> core;
  std::shared_ptr<Taskpool> tasks;
  std::shared_ptr<OutboundConversations> outbounds;

  Threadpool comms_runners;
  Threadpool tasks_runners;

  void startListeners(const std::vector<int> &ports);

  MtCore(const MtCore &other) = delete;
  MtCore &operator=(const MtCore &other) = delete;
  bool operator==(const MtCore &other) = delete;
  bool operator<(const MtCore &other) = delete;
};
