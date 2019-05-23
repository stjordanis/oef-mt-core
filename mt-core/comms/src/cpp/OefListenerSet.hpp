#pragma once

#include <map>
#include <mutex>

#include "mt-core/comms/src/cpp/IOefListener.hpp"

class OefListenerSet
{
public:
  using ListenerId = int;
  using Listener = IOefListener;
  using ListenerP = std::shared_ptr<Listener>;
  using Store = std::map<ListenerId, ListenerP>;
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  OefListenerSet()
  {
  }
  virtual ~OefListenerSet()
  {
  }

  bool has(const ListenerId &id) const
  {
    Lock lock(mutex);
    return store.find(id) != store.end();
  }

  bool add(const ListenerId &id, ListenerP new_listener)
  {
    Lock lock(mutex);
    if (store.find(id) == store.end())
    {
      return false;
    }
    store[id] = new_listener;
    return true;
  }

  void clear(void)
  {
    store.clear();
  }
protected:
private:
  mutable Mutex mutex;
  Store store;

  OefListenerSet(const OefListenerSet &other) = delete;
  OefListenerSet &operator=(const OefListenerSet &other) = delete;
  bool operator==(const OefListenerSet &other) = delete;
  bool operator<(const OefListenerSet &other) = delete;
};
