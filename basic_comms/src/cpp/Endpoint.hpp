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
    ,asio_sending(false)
    ,asio_reading(false)
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

  std::shared_ptr<IMessageReader> reader;
  std::shared_ptr<IMessageWriter> writer;

  RingBuffer sendBuffer;
  RingBuffer readBuffer;

  void run_sending()
  {
    {
      Lock lock(mutex);
      if (asio_sending)
      {
        return;
      }
      if (sendBuffer.getDataAvailable() == 0)
      {
        create_messages();
      }
      if (sendBuffer.getDataAvailable() == 0)
      {
        return;
      }
      asio_sending = true;
    }

    auto data = sendBuffer.getDataBuffers();
    boost::asio::async_write(
          sock,
          data,
          [this](const boost::system::error_code& ec, const size_t &bytes){
              this -> complete_sending(ec, bytes);
          }
      );
  }
  void run_reading()
  {
    std::size_t read_needed_local = 0;

    {
      Lock lock(mutex);
      if (asio_reading)
      {
        return;
      }
      if (read_needed == 0)
      {
        return;
      }
      if (read_needed > readBuffer.getFreeSpace())
      {
        return;
      }
      read_needed_local = read_needed;
      read_needed = 0;
      asio_reading = true;
    }

    auto space = readBuffer.getSpaceBuffers();
    boost::asio::async_read(
                            sock,
                            space,
                            boost::asio::transfer_at_least(read_needed_local),
                            [this](const boost::system::error_code& ec, const size_t &bytes){
                              this -> complete_reading(ec, bytes);
                            }
                            );
  }

private:
  Mutex mutex;
  std::size_t read_needed = 0;

  std::atomic<bool> asio_sending;
  std::atomic<bool> asio_reading;


  void complete_sending(const boost::system::error_code& ec, const size_t &bytes)
  {
    {
      Lock lock(mutex);
      asio_sending = false;
    }
    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      std::cerr << "EOF" << ec << std::endl;
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    if (ec)
    {
      std::cout << "ERROR:" << ":" << ec.message() << std::endl;
      exit(0); // TODO(KLL): Tell someone.
      return;
    }

    sendBuffer.markDataUsed(bytes);
    create_messages();
    run_sending();
  }

  void create_messages()
  {
    auto consumed_needed = writer -> checkForSpace(sendBuffer.getSpaceBuffers());
    auto consumed = consumed_needed.first;
    sendBuffer.markSpaceUsed(consumed);
  }

  void complete_reading(const boost::system::error_code& ec, const size_t &bytes)
  {
    {
      Lock lock(mutex);
      asio_reading = false;
    }
    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      std::cerr << "EOF" << ec << std::endl;
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
