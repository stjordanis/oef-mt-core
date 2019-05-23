#pragma once

#include <memory>

#include "mt-core/comms/src/cpp/IOefListener.hpp"
#include "basic_comms/src/cpp/Listener.hpp"

class Core;

class Oefv1Listener:public IOefListener
{
public:
  Oefv1Listener(std::shared_ptr<Core> core, int port);
  virtual ~Oefv1Listener()
  {
  }

protected:
private:
  Listener listener;

  Oefv1Listener(const Oefv1Listener &other) = delete;
  Oefv1Listener &operator=(const Oefv1Listener &other) = delete;
  bool operator==(const Oefv1Listener &other) = delete;
  bool operator<(const Oefv1Listener &other) = delete;
};
