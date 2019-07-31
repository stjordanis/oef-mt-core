#pragma once

#include "mt-core/tasks-base/src/cpp/IMtCoreTask.hpp"
#include "mt-core/comms/src/cpp/IOefListener.hpp"
#include "basic_comms/src/cpp/Core.hpp"

#include <unordered_map>

class OefListenerSet;
class IKarmaPolicy;

template <template <typename> class EndpointType>
class OefListenerStarterTask:public IMtCoreTask
{
public:
  using FactoryCreator = IOefListener::FactoryCreator;
  using ConfigMap      = std::unordered_map<std::string, std::string>;

  OefListenerStarterTask(int p,
                         std::shared_ptr<OefListenerSet> listeners,
                         std::shared_ptr<Core> core,
                         FactoryCreator initialFactoryCreator,
                         IKarmaPolicy *karmaPolicy,
                         ConfigMap endpointConfig)
  {
    this -> p = p;
    this -> listeners = listeners;
    this -> core = core;
    this -> initialFactoryCreator = initialFactoryCreator;
    this -> karmaPolicy = karmaPolicy;
    this -> endpointConfig = std::move(endpointConfig);
  }
  virtual ~OefListenerStarterTask()
  {
  }

  virtual bool isRunnable(void) const
  {
    return true;
  }
  virtual ExitState run(void);
protected:
private:
  std::shared_ptr<OefListenerSet> listeners;
  std::shared_ptr<Core> core;
  int p;
  FactoryCreator initialFactoryCreator;
  IKarmaPolicy *karmaPolicy;
  ConfigMap endpointConfig;

  OefListenerStarterTask(const OefListenerStarterTask &other) = delete;
  OefListenerStarterTask &operator=(const OefListenerStarterTask &other) = delete;
  bool operator==(const OefListenerStarterTask &other) = delete;
  bool operator<(const OefListenerStarterTask &other) = delete;
};
