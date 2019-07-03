#pragma once

#include "threading/src/cpp/lib/Waitable.hpp"
#include "threading/src/cpp/lib/Notification.hpp"

#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include <iostream>
#include <list>

class Uri;

template <typename TXType>
class EndpointBase
  : public ISocketOwner
  , public Waitable
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

  static constexpr std::size_t BUFFER_SIZE_LIMIT = 50;
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
  std::shared_ptr<IMessageWriter<TXType>> writer;

  ErrorNotification onError;
  EofNotification onEof;
  StartNotification onStart;
  ProtoErrorNotification onProtoError;

  virtual void run_sending();
  virtual void run_reading();
  virtual void close();
  virtual bool connect(const Uri &uri, Core &core);

  virtual const std::string &getRemoteId() const { return remote_id; }

  virtual Notification::NotificationBuilder send(TXType &s);

  virtual bool IsTXQFull()
  {
    Lock lock(txq_mutex);
    return txq.size() >= BUFFER_SIZE_LIMIT;
  }

  virtual bool connected()
  {
    if (*state>RUNNING_ENDPOINT) {
      FETCH_LOG_INFO(LOGGING_NAME, "STATE: ", state);
    }
    return *state== RUNNING_ENDPOINT;
  }

  std::size_t getIdent(void) const { return ident; }

protected:

  virtual void async_read(const std::size_t& bytes_needed) = 0;
  virtual void async_write() = 0;

  virtual bool is_eof(const boost::system::error_code& ec) const = 0;

protected:
  RingBuffer sendBuffer;
  RingBuffer readBuffer;

  Mutex mutex;
  Mutex txq_mutex;
  std::size_t read_needed = 0;
  std::size_t ident;

  std::string remote_id;

  std::atomic<bool> asio_sending;
  std::atomic<bool> asio_reading;

  std::shared_ptr<StateType> state;

  virtual void error(const boost::system::error_code& ec);
  virtual void proto_error(const std::string &msg);
  virtual void eof();


  virtual void complete_sending(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes);
  virtual void create_messages();
  virtual void complete_reading(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes);

private:
  std::vector<Notification::Notification> waiting;
  std::list<TXType> txq;
};
