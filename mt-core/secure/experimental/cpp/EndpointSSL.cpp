#include "EndpointSSL.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"

#include <memory>

static Gauge ep_count("mt-core.network.EndpointSSL");

template <typename TXType>
EndpointSSL<TXType>::EndpointSSL(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
      ,std::string key 
  )
    : Endpoint<TXType>(core, sendBufferSize, readBufferSize)
    , key_(key), sock(core)
{
  ep_count++;
  ssl_setup = false;
  try 
  {
    ssl_ctx_p = make_ssl_ctx();
    ssl_sock_p = new SocketSSL(std::move(sock), *ssl_ctx_p);
  }
  catch(std::exception &ec)
  {
    FETCH_LOG_ERROR(LOGGING_NAME, "SSL context initialization: ", ec.what());
    return;
  }
  ssl_setup = true;
}

template <typename TXType>
EndpointSSL<TXType>::~EndpointSSL()
{
  FETCH_LOG_INFO(LOGGING_NAME, "~EndpointSSL<>");
  ep_count--;
  delete ssl_ctx_p;
  delete ssl_sock_p;
}

template <typename TXType>
void EndpointSSL<TXType>::close()
{
  if(!ssl_setup)
    return;

  Lock lock(this->mutex);
  *state |= this->CLOSED_ENDPOINT;
  try
  {
    ssl_sock_p->shutdown();
    ssl_sock_p->lowest_layer().shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);

    ssl_sock_p->lowest_layer().close();
  }
  catch (std::exception &ec)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "SSL Socket when closing: ", ec.what());
  }
}

template <typename TXType>
void EndpointSSL<TXType>::go()
{
  FETCH_LOG_WARN(LOGGING_NAME, "Got new connection, attempting ssl handshake ...");
  ssl_sock_p->async_handshake(boost::asio::ssl::stream_base::server, 
    [this](const boost::system::error_code& error)
      {
        if (!error)
        {
          FETCH_LOG_WARN(LOGGING_NAME, "SSL handshake successfull");
          Endpoint<TXType>::go();
        }
        else
        {
          FETCH_LOG_ERROR(LOGGING_NAME, "SSL handshake failed: ", error.message());
          close();
        }
      });
}

template <typename TXType>
void EndpointSSL<TXType>::async_write()
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
                           *ssl_sock_p,
                           data,
                           [this, my_state](const  boost::system::error_code& ec, const size_t &bytes){
                             this -> complete_sending(my_state, ec, bytes);
                           }
                           );
}

template <typename TXType>
void EndpointSSL<TXType>::async_read(const std::size_t& bytes_needed)
{
  auto space = readBuffer.getSpaceBuffers();
  auto my_state = state;

  boost::asio::async_read(
                          *ssl_sock_p,
                          space,
                          boost::asio::transfer_at_least(bytes_needed),
                          [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
                            this -> complete_reading(my_state, ec, bytes);
                          }
                          );
}

template <typename TXType>
std::string EndpointSSL<TXType>::key_get_password() const
{
  return "test";
}

template <typename TXType>
typename EndpointSSL<TXType>::ContextSSL* EndpointSSL<TXType>::make_ssl_ctx()
{
  // TOFIX(LR) hard coded 
  auto ssl_ctx = new ContextSSL(boost::asio::ssl::context::sslv23);
  ssl_ctx->set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use);
  ssl_ctx->set_password_callback(std::bind(&EndpointSSL::key_get_password, this));
  ssl_ctx->use_certificate_chain_file("mt-core/secure/experimental/cpp/server.pem");
  ssl_ctx->use_private_key_file("mt-core/secure/experimental/cpp/server.pem", boost::asio::ssl::context::pem);
  ssl_ctx->use_tmp_dh_file("mt-core/secure/experimental/cpp/dh2048.pem");
  
  return ssl_ctx;
}

template class EndpointSSL<std::shared_ptr<google::protobuf::Message>>;
template class EndpointSSL<std::pair<Uri, std::shared_ptr<google::protobuf::Message>>>;
