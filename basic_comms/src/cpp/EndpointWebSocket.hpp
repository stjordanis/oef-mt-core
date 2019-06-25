#pragma once

#include "basic_comms/src/cpp/EndpointBase.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "basic_comms/src/cpp/RingBuffer.hpp"

#include "boost/beast.hpp"
#include "boost/beast/core.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/strand.hpp"

#include <memory>

class EndpointWebSocket
    : public EndpointBase
    , public std::enable_shared_from_this<EndpointWebSocket>
{
public:
  using Socket     = EndpointBase::Socket ;
  using Lock       = EndpointBase::Lock;
  using StateType  = EndpointBase::StateType;
  using WebSocket = boost::beast::websocket::stream<Socket>;

  static constexpr char const *LOGGING_NAME = "EndpointWebSocket";

  explicit EndpointWebSocket(
      boost::asio::io_context& io_context,
      std::size_t sendBufferSize,
      std::size_t readBufferSize);

  virtual ~EndpointWebSocket();

  EndpointWebSocket(const EndpointWebSocket &other) = delete;
  EndpointWebSocket &operator=(const EndpointWebSocket &other) = delete;
  bool operator==(const EndpointWebSocket &other) = delete;
  bool operator<(const EndpointWebSocket &other) = delete;

  virtual Socket& socket() override
  {
    return web_socket_.next_layer();
  }

  virtual void close() override;
  virtual void go() override;

protected:
  virtual void async_read(const std::size_t& bytes_needed) override;
  virtual void async_write() override;

  void async_read_at_least(
      const std::size_t& bytes_needed,
      std::size_t bytes_read,
      std::vector<RingBuffer::mutable_buffer>& space,
      std::shared_ptr<StateType> my_state
  );

private:
  void on_accept(const boost::system::error_code& ec);

private:
  WebSocket web_socket_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
};
