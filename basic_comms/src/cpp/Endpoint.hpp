#pragma once

#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"

class Endpoint:public ISocketOwner
{
public:
  using Socket = boost::asio::ip::tcp::socket;

  Endpoint(const Endpoint &other) = delete;
  Endpoint &operator=(const Endpoint &other) = delete;
  bool operator==(const Endpoint &other) = delete;
  bool operator<(const Endpoint &other) = delete;

  Endpoint(Core &core):sock(core)
  {
  }
  virtual ~Endpoint()
  {
  }

  virtual Socket& socket()
  {
    return sock;
  }

  virtual void go()
  {
    run_writing();
    run_reading();
  }
private:

  void run_writing()
  {
  }
  void run_reading()
  {
  }
  Socket sock;
};
