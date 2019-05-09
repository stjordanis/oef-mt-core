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

void Core::runThreaded(std::size_t threadcount)
{
  threads.reserve(threadcount);
  for (std::size_t thread_idx = 0; thread_idx < threadcount; ++thread_idx)
  {
    threads.emplace_back(std::make_shared<std::thread>([this]() { context -> run(); }));
  }
}

void Core::stop()
{
  context -> stop();
  for(auto &thr : threads)
  {
    thr->join();
  }
  threads.empty();
}

std::shared_ptr<tcp::acceptor> Core::makeAcceptor(unsigned short int port)
{
  return std::make_shared<tcp::acceptor>(*context, tcp::endpoint(tcp::v4(), port));
}
