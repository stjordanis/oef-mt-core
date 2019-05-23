#pragma once

#include "mt-core/tasks-base/src/cpp/IMtCoreTask.hpp"
#include "basic_comms/src/cpp/Core.hpp"

class OefListenerSet;

class OefListenerStarterTask:public IMtCoreTask
{
public:
  OefListenerStarterTask(int p, std::shared_ptr<OefListenerSet> listeners, std::shared_ptr<Core> core)
  {
    this -> p = p;
    this -> listeners = listeners;
    this -> core = core;
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
  OefListenerStarterTask(const OefListenerStarterTask &other) = delete;
  OefListenerStarterTask &operator=(const OefListenerStarterTask &other) = delete;
  bool operator==(const OefListenerStarterTask &other) = delete;
  bool operator<(const OefListenerStarterTask &other) = delete;
};
