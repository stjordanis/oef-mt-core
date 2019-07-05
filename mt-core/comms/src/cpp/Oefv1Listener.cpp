#include "Oefv1Listener.hpp"

#include <memory>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

class IKarmaPolicy;

Oefv1Listener::Oefv1Listener(std::shared_ptr<Core> core, int port, IKarmaPolicy *karmaPolicy):listener(*core, port)
{
  this -> port = port;
  this -> karmaPolicy = karmaPolicy;
  listener.creator = [this](Core &core){
    auto ep = std::make_shared<OefAgentEndpoint>(core);
    ep -> setup(ep, this->karmaPolicy);
    auto factory = this -> factoryCreator(ep);
    ep -> setFactory(factory);
    return ep;
  };
}

void Oefv1Listener::start(void)
{
  listener.start_accept();
}
