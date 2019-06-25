#include "Endpoint.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include <cstdlib>
#include "monitoring/src/cpp/lib/Gauge.hpp"

static Gauge ep_count("mt-core.network.Endpoint");

Endpoint::Endpoint(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
  )
    : EndpointBase(sendBufferSize, readBufferSize)
    , sock(core)
{
  ep_count++;
}

Endpoint::~Endpoint()
{
  ep_count--;
}


void Endpoint::async_write()
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
  boost::asio::async_write(
                           sock,
                           data,
                           [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
                             this -> complete_sending(state, ec, bytes);
                           }
                           );
}

void Endpoint::async_read(const std::size_t& bytes_needed)
{
  auto space = readBuffer.getSpaceBuffers();
  auto my_state = state;

  boost::asio::async_read(
                          sock,
                          space,
                          boost::asio::transfer_at_least(bytes_needed),
                          [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
                            this -> complete_reading(state, ec, bytes);
                          }
                          );
}
