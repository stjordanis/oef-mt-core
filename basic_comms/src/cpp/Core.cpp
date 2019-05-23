#include "Core.hpp"

#include <iostream>

Core::Core()
{
  context = std::make_shared<boost::asio::io_context>();
  work = new boost::asio::io_context::work(*context);
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
  delete work;
  context -> stop();
}

std::shared_ptr<tcp::acceptor> Core::makeAcceptor(unsigned short int port)
{
  return std::make_shared<tcp::acceptor>(*context, tcp::endpoint(tcp::v4(), port));
}
