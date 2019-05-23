#include "Oefv1Listener.hpp"

#include <memory>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

Oefv1Listener::Oefv1Listener(std::shared_ptr<Core> core, int port):listener(*core, 7600)
{
  listener.creator = [](Core &core){
    return std::make_shared<OefAgentEndpoint>(core);
  };
  listener.start_accept();
}

