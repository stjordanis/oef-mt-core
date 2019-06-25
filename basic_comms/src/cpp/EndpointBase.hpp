#pragma once

#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include <iostream>

class Uri;

class EndpointBase
  : public ISocketOwner
{
public:
  using Mutex   = std::mutex;
  using Lock    = std::lock_guard<Mutex>;
  using Socket  = boost::asio::ip::tcp::socket;

  using ErrorNotification      = std::function<void (const boost::system::error_code& ec)> ;
  using EofNotification        = std::function<void ()> ;
  using StartNotification      = std::function<void ()> ;
  using ProtoErrorNotification = std::function<void (const std::string &message)> ;

  using StateType  = std::atomic<int>;
  using StateTypeP = std::shared_ptr<StateType>;
  

  using EndpointState = enum {
    RUNNING_ENDPOINT = 1,
    CLOSED_ENDPOINT = 2,
    EOF_ENDPOINT = 4,
    ERRORED_ENDPOINT = 8
  };

  static constexpr char const *LOGGING_NAME = "EndpointBase";

  EndpointBase(const EndpointBase &other) = delete;
  EndpointBase &operator=(const EndpointBase &other) = delete;
  bool operator==(const EndpointBase &other) = delete;
  bool operator<(const EndpointBase &other) = delete;

  EndpointBase(
       std::size_t sendBufferSize
      ,std::size_t readBufferSize
  );
  virtual ~EndpointBase();

  virtual Socket& socket() = 0;

  virtual void go();

  std::shared_ptr<IMessageReader> reader;
  std::shared_ptr<IMessageWriter> writer;

  ErrorNotification onError;
  EofNotification onEof;
  StartNotification onStart;
  ProtoErrorNotification onProtoError;

  virtual void run_sending();
  virtual void run_reading();
  virtual void close();
  virtual bool connect(const Uri &uri, Core &core);

  const std::string &getRemoteId() const { return remote_id; }

protected:

  virtual void async_read(const std::size_t& bytes_needed) = 0;
  virtual void async_write() = 0;

protected:
  RingBuffer sendBuffer;
  RingBuffer readBuffer;

  Mutex mutex;
  std::size_t read_needed = 0;

  std::string remote_id;

  std::atomic<bool> asio_sending;
  std::atomic<bool> asio_reading;

  std::shared_ptr<StateType> state;

  void error(const boost::system::error_code& ec);
  void proto_error(const std::string &msg);
  void eof();


  void complete_sending(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes);
  void create_messages();
  void complete_reading(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes);
};
