#pragma once

#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Core
{
public:
  Core();
  virtual ~Core();

  void run(void);
  void stop(void);

  operator boost::asio::io_context*() { return context.get(); }
  operator boost::asio::io_context&() { return *context; }

  std::shared_ptr<tcp::acceptor> makeAcceptor(unsigned short int port);
private:

  std::shared_ptr<boost::asio::io_context> context;
  using ThreadP  = std::shared_ptr<std::thread>;
  using Threads = std::vector<ThreadP>;
  Threads threads;
};
