#pragma once

#include <memory>
#include <iostream>

#include "mt-core/comms/src/cpp/IOefListener.hpp"
#include "basic_comms/src/cpp/Listener.hpp"

class Core;
class OefAgentEndpoint;
class IOefAgentTaskFactory;

class Oefv1Listener:public IOefListener
{
public:
  Oefv1Listener(std::shared_ptr<Core> core, int port);
  virtual ~Oefv1Listener()
  {
    std::cout << "Listener on "<< port << " GONE" << std::endl;
  }

  void start(void);
protected:
private:
  Listener listener;
  int port;

  Oefv1Listener(const Oefv1Listener &other) = delete;
  Oefv1Listener &operator=(const Oefv1Listener &other) = delete;
  bool operator==(const Oefv1Listener &other) = delete;
  bool operator<(const Oefv1Listener &other) = delete;
};
