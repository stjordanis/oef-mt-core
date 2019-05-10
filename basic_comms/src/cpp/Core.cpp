#include "Core.hpp"

Core::Core()
{
  context = std::make_shared<boost::asio::io_context>();
}

Core::~Core()
{
  stop();
}

void Core::run()
{
  context -> run();
}

void Core::stop()
{
  context -> stop();
}

std::shared_ptr<tcp::acceptor> Core::makeAcceptor(unsigned short int port)
{
  return std::make_shared<tcp::acceptor>(*context, tcp::endpoint(tcp::v4(), port));
}
