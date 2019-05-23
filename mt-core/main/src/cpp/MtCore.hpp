#pragma once

#include <vector>

class OefListenerSet;
class Core;

class MtCore
{
public:
  using args = struct
  {
    std::vector<int> listen_ports;
  };

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

  void startListeners(const std::vector<int> &ports);

  MtCore(const MtCore &other) = delete;
  MtCore &operator=(const MtCore &other) = delete;
  bool operator==(const MtCore &other) = delete;
  bool operator<(const MtCore &other) = delete;
};
