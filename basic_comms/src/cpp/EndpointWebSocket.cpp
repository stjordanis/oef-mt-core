#include "EndpointWebSocket.hpp"

#include "boost/asio/bind_executor.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"

#include <utility>

namespace websocket = boost::beast::websocket;

static Gauge wsep_count("mt-core.network.EndpointWebSocket");


EndpointWebSocket::EndpointWebSocket(
    boost::asio::io_context& io_context,
    std::size_t sendBufferSize,
    std::size_t readBufferSize)
  : EndpointBase(sendBufferSize, readBufferSize)
  , web_socket_(io_context)
  , strand_(web_socket_.get_executor())
{
  wsep_count ++;
}

EndpointWebSocket::~EndpointWebSocket()
{
  wsep_count --;
}


void EndpointWebSocket::close()
{
  Lock lock(mutex);
  *state |= CLOSED_ENDPOINT;
  web_socket_.close(websocket::close_code::normal);
}


void EndpointWebSocket::go()
{
  web_socket_.async_accept(
      boost::asio::bind_executor(
          strand_,
          std::bind(
              &EndpointWebSocket::on_accept,
              shared_from_this(),
              std::placeholders::_1
          )
      )
  );
}

void EndpointWebSocket::async_write()
{
  auto data = sendBuffer.getDataBuffers();

  int i = 0;
  for(auto &d : data)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Send buffer ", i, "=", d.size(), " bytes on thr=", std::this_thread::get_id());
    ++i;
  }

  FETCH_LOG_INFO(LOGGING_NAME, "run_sending: START");

  auto my_state = state;

  web_socket_.async_write(
      data,
      [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
        this -> complete_sending(state, ec, bytes);
      }
  );
}

void EndpointWebSocket::async_read(const std::size_t& bytes_needed)
{
  auto space = readBuffer.getSpaceBuffers();
  auto my_state = state;
  async_read_at_least(bytes_needed, 0, space, my_state);
}


void EndpointWebSocket::async_read_at_least(
    const std::size_t& bytes_needed,
    std::size_t bytes_read,
    std::vector<RingBuffer::mutable_buffer>& space,
    std::shared_ptr<StateType> my_state
)
{
  if (bytes_read >= bytes_needed)
  {
    return;
  }

  web_socket_.async_read_some(
      space,
      [this, my_state, &bytes_read, bytes_needed, &space](const boost::system::error_code& ec, const size_t &bytes){
        bytes_read += bytes;
        if (bytes_read >= bytes_needed)
        {
          this->complete_reading(state, ec, bytes);
        }
        else
        {
          FETCH_LOG_INFO(LOGGING_NAME,
              "Calling read_at_least again, bytes_needed: ", bytes_needed, ", bytes_read: ", bytes_read);
          this->async_read_at_least(bytes_needed, bytes_read, space, my_state);
        }
      }
  );
}


void EndpointWebSocket::on_accept(const boost::system::error_code& ec)
{
  EndpointBase::go();
/*
  using namespace std::chrono_literals;
  if (ec)
  {
    FETCH_LOG_WARN("Error accepting web socket: ", ec.message());
    return;
  }
  //boost::beast::multi_buffer buffer;
  // Read a message into our buffer
  //web_socket_.read(buffer);

  auto space = readBuffer.getSpaceBuffers();
  boost::system::error_code errorCode;

  FETCH_LOG_INFO(LOGGING_NAME, "Reading...");
  auto bytes = web_socket_.read_some(space, errorCode);
  readBuffer.markSpaceUsed(bytes);
  FETCH_LOG_WARN(LOGGING_NAME, "Got data: ", bytes, " , ", readBuffer.getDataAvailable());

  if (errorCode) {
    FETCH_LOG_WARN(LOGGING_NAME, "Error reading web socket: ", ec.message());
    return;
  }


  FETCH_LOG_INFO(LOGGING_NAME, "Getting data");
  auto data = readBuffer.getDataBuffers();

  FETCH_LOG_INFO(LOGGING_NAME, "Write data: ", data.size(), ". data[0].size=", data[0].size());
  auto written = web_socket_.write(data, errorCode);

  if (errorCode) {
    FETCH_LOG_WARN(LOGGING_NAME, "Error writing web socket: ", ec.message());
    return;
  }

  FETCH_LOG_INFO(LOGGING_NAME, "Data written: ", written);
*/

}