#include <functional>

#include "Listener.hpp"
#include "Core.hpp"

Listener::Listener(Core &thecore, unsigned short int port):core(thecore)
{
  acceptor = thecore.makeAcceptor(port);
}

void Listener::start_accept()
{
  auto new_connection = creator(core);
  acceptor -> async_accept(new_connection->socket(),
                           std::bind(&Listener::handle_accept, this, new_connection, std::placeholders::_1));
}

void Listener::handle_accept(std::shared_ptr<ISocketOwner> new_connection, const boost::system::error_code& error)
{
  if (!error)
  {
    new_connection->go();
    start_accept();
  }
}

Listener::~Listener()
{
}
