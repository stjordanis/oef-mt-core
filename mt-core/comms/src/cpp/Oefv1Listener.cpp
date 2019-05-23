#include "Oefv1Listener.hpp"

#include <memory>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

Oefv1Listener::Oefv1Listener(std::shared_ptr<Core> core, int port):listener(*core, port)
{
  std::cout << "Oefv1Listener::Oefv1Listener" << std::endl;
  this -> port = port;
  listener.creator = [this](Core &core){
    auto ep = std::make_shared<OefAgentEndpoint>(core);
    auto factory = this -> factoryCreator(ep);
    ep -> setFactory(factory);
    std::cout << "Oefv1Listener:: running on port " << this -> port << std::endl;
    return ep;
  };
  std::cout << "Oefv1Listener::Oefv1Listener created" << std::endl;
}

void Oefv1Listener::start(void)
{
  listener.start_accept();
  std::cout << "Oefv1Listener::Oefv1Listener started" << std::endl;
}
