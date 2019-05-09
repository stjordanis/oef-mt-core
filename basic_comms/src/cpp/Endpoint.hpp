#pragma once

#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include <iostream>

class Endpoint:public ISocketOwner
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using Socket = boost::asio::ip::tcp::socket;

  Endpoint(const Endpoint &other) = delete;
  Endpoint &operator=(const Endpoint &other) = delete;
  bool operator==(const Endpoint &other) = delete;
  bool operator<(const Endpoint &other) = delete;

  Endpoint(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
  )
    :sock(core)
    ,sendBuffer(sendBufferSize)
    ,readBuffer(readBufferSize)
  {
  }
  virtual ~Endpoint()
  {
  }

  virtual Socket& socket()
  {
    return sock;
  }

  virtual void eof()
  {
    //TODO(kll): tell someone
  }

  virtual void go()
  {
    {
      Lock lock(mutex);
      read_needed++;
    }

    run_reading();
  }

  Socket sock;

  IMessageReader *reader;
  IMessageWriter *writer;

  RingBuffer sendBuffer;
  RingBuffer readBuffer;

private:

  Mutex mutex;
  std::size_t send_needed = 0;
  std::size_t read_needed = 0;

  void run_sending()
  {
    Lock lock(mutex);
    if (send_needed > 0)
    {
      send_needed = 0;
      // start ASIO here.
    }
  }
  void run_reading()
  {
    Lock lock(mutex);
    if (read_needed > 0)
    {
      auto read_needed_local = read_needed;
      read_needed = 0;
      // start ASIO here.

      auto space = readBuffer.getSpaceBuffers();
      //for(auto const& b : space)
      //{
      //  std::cerr << "SIZE=" << b.size() << std::endl;
      //}

      boost::asio::async_read(
          sock,
          space,
          boost::asio::transfer_at_least(read_needed_local),
          [this](const boost::system::error_code& ec, const size_t &bytes){
              this -> complete_reading(ec, bytes);
          }
      );
    }
  }

  void complete_sending(const boost::system::error_code& ec, const size_t &bytes)
  {
    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    if (ec)
    {
      std::cout << "ERROR:" << ":" << ec.message() << std::endl;
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    //TODO(KLL): Mark space.
    //TODO(KLL): Call message writer to fill space.
  }
  void complete_reading(const boost::system::error_code& ec, const size_t &bytes)
  {
    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    if (ec)
    {
      std::cout << "ERROR:" << ":" << ec.message() << std::endl;
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    readBuffer.markSpaceUsed(bytes);

    auto consumed_needed = reader -> checkForMessage(readBuffer.getDataBuffers());
    auto consumed = consumed_needed.first;
    auto needed = consumed_needed.second;

    readBuffer.markDataUsed(consumed);

    {
      Lock lock(mutex);
      read_needed = needed;
    }

    run_reading();
  }
};
