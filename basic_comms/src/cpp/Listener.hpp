#pragma once
#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/ISocketOwner.hpp"

using boost::asio::ip::tcp;

class Core;

class Listener
{
public:
  using CONN_CREATOR = std::function< ISocketOwner* (Core &core)>;


  Listener(Core &core);
  virtual ~Listener();

  void start_accept();
  void handle_accept(ISocketOwner *new_connection, const boost::system::error_code& error);

  std::shared_ptr<tcp::acceptor> acceptor;
  CONN_CREATOR creator;
private:
  Core &core;
};
