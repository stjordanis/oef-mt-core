#pragma once

#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include <iostream>

class Uri;

class Endpoint:public ISocketOwner
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using Socket = boost::asio::ip::tcp::socket;

  using ErrorNotification = std::function<void (const boost::system::error_code& ec)> ;
  using EofNotification = std::function<void ()> ;
  using StartNotification = std::function<void ()> ;
  using ProtoErrorNotification = std::function<void (const std::string &message)> ;

  using EndpointState = enum {
    RUNNING_ENDPOINT = 1,
    CLOSED_ENDPOINT = 2,
    EOF_ENDPOINT = 4,
    ERRORED_ENDPOINT = 8
  };

  Endpoint(const Endpoint &other) = delete;
  Endpoint &operator=(const Endpoint &other) = delete;
  bool operator==(const Endpoint &other) = delete;
  bool operator<(const Endpoint &other) = delete;

  Endpoint(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
  );
  virtual ~Endpoint();

  virtual Socket& socket()
  {
    return sock;
  }

  virtual void go();

  std::shared_ptr<IMessageReader> reader;
  std::shared_ptr<IMessageWriter> writer;

  ErrorNotification onError;
  EofNotification onEof;
  StartNotification onStart;
  ProtoErrorNotification onProtoError;

  void run_sending();
  void run_reading();
  void close();
  bool connect(const Uri &uri, Core &core);

  const std::string &getRemoteId() const { return remote_id; }
protected:
  Socket sock;
  RingBuffer sendBuffer;
  RingBuffer readBuffer;

  Mutex mutex;
  std::size_t read_needed = 0;

  std::string remote_id;

  std::atomic<bool> asio_sending;
  std::atomic<bool> asio_reading;

  std::atomic<int> state;

  void error(const boost::system::error_code& ec);
  void proto_error(const std::string &msg);
  void eof();


  void complete_sending(const boost::system::error_code& ec, const size_t &bytes);
  void create_messages();
  void complete_reading(const boost::system::error_code& ec, const size_t &bytes);
};
