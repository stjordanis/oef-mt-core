#include "Oefv1Listener.hpp"

#include <memory>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "basic_comms/src/cpp/EndpointWebSocket.hpp"

template <template <typename> class EndpointType>
Oefv1Listener<EndpointType>::Oefv1Listener(std::shared_ptr<Core> core, int port, IKarmaPolicy *karmaPolicy):listener(*core, port)
{
  this -> port = port;
  this -> karmaPolicy = karmaPolicy;
  listener.creator = [this](Core &core) {
    std::cout << "Create endpoint...." << std::endl;
    auto ep0 = std::make_shared<EndpointType<std::shared_ptr<google::protobuf::Message>>>(core, 1000000, 1000000);
    auto ep1 = std::make_shared<ProtoMessageEndpoint>(std::move(ep0));
    ep1->setup(ep1);
    auto ep2 = std::make_shared<OefAgentEndpoint>(std::move(ep1));
    auto factory = this -> factoryCreator(ep2);
    ep2 -> setFactory(factory);
    ep2 -> setup(this -> karmaPolicy);
    return ep2;
  };
}

template <template <typename> class EndpointType>
void Oefv1Listener<EndpointType>::start(void)
{
  listener.start_accept();
}

template class Oefv1Listener<Endpoint>;
template class Oefv1Listener<EndpointWebSocket>;
