#include "Oefv1Listener.hpp"

#include <memory>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

Oefv1Listener::Oefv1Listener(std::shared_ptr<Core> core, int port):listener(*core, port)
{
  this -> port = port;
  listener.creator = [this](Core &core){
    auto ep0 = std::make_shared<Endpoint<std::shared_ptr<google::protobuf::Message>>>(core, 1000000, 1000000);
    auto ep1 = std::make_shared<ProtoMessageEndpoint>(std::move(ep0));
    ep1->setup(ep1);
    auto ep2 = std::make_shared<OefAgentEndpoint>(std::move(ep1));
    auto factory = this -> factoryCreator(ep2);
    ep2 -> setFactory(factory);
    ep2 -> setup();
    return ep2;
  };
}

void Oefv1Listener::start(void)
{
  listener.start_accept();
}
