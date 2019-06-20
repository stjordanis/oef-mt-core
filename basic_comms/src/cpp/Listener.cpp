#include <functional>
#include <iostream>

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
  std::cout << "Listener::handle_accept " << std::endl;
  if (!error)
  {
    new_connection->go();
    start_accept();
  }
  else
  {
    std::cout << "Listener::handle_accept " << error << std::endl;
  }
}

Listener::~Listener()
{
}
