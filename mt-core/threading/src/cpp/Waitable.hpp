#pragma once

#include <mutex>
#include "threading/src/cpp/lib/Notification.hpp"
#include <vector>

class Waitable
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using Waiting = std::vector<Notification::Notification>;

  Waitable()
  {
  }
  virtual ~Waitable()
  {
  }

  Notification::NotificationBuilder makeNotification(void);
  void wake(void);

protected:
  Waiting waiting;
  mutable Mutex mutex;
private:
  Waitable(const Waitable &other) = delete; // { copy(other); }
  Waitable &operator=(const Waitable &other) = delete; // { copy(other); return *this; }
  bool operator==(const Waitable &other) = delete; // const { return compare(other)==0; }
  bool operator<(const Waitable &other) = delete; // const { return compare(other)==-1; }
};
