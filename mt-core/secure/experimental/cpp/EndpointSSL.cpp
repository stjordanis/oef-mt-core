#include "EndpointSSL.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"

#include <openssl/ssl.h>
#include <cerrno> // for fopen
#include <cstdio> // for fopen
#include <memory>
#include <functional>

using std::placeholders::_1;
using std::placeholders::_2;

static Gauge ep_count("mt-core.network.EndpointSSL");

template <typename TXType>
EndpointSSL<TXType>::EndpointSSL(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
      ,std::string sk_file
      ,std::string pk_file
  )
    : Endpoint<TXType>(core, sendBufferSize, readBufferSize)
    , sock(core)
    , sk_f(sk_file)
    , pk_f(pk_file)
{
  ep_count++;
  ssl_setup = false;
  try 
  {
    ssl_ctx_p = make_ssl_ctx();
    ssl_sock_p = new SocketSSL(std::move(sock), *ssl_ctx_p);
    ssl_sock_p-> set_verify_mode(boost::asio::ssl::verify_peer);
    ssl_sock_p-> set_verify_callback(
        std::bind(&verify_agent_certificate, _1, _2));
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
          try
          {
            X509CertP cert{ssl_sock_p->native_handle()};
            EvpPublicKey pk{cert};
            
            agent_ssl_key_ = RSA_Modulus(pk);
            FETCH_LOG_INFO(LOGGING_NAME, "Got Agent PubKey: ", agent_ssl_key_);
          }
          catch (std::exception& e)
          {
            FETCH_LOG_WARN(LOGGING_NAME, "Couldn't get agent public key from ssl socket : ", e.what());
            return;
          }
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
std::shared_ptr<EvpPublicKey> EndpointSSL<TXType>::get_peer_ssl_key()
{
  X509CertP cert{ssl_sock_p->native_handle()};
  return std::make_shared<EvpPublicKey>(cert);
}
  
bool verify_agent_certificate(bool preverified,
      boost::asio::ssl::verify_context& ctx)
{
  // Accept any
  char subject_name[256];
  X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
  X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
  FETCH_LOG_INFO("EndpointSSL", "Certificate: ", subject_name);

  return true;
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

  FETCH_LOG_INFO(LOGGING_NAME, "run_reading: START");
  
  //auto self = shared_from_this();
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
typename EndpointSSL<TXType>::ContextSSL* EndpointSSL<TXType>::make_ssl_ctx()
{
  auto ssl_ctx = new ContextSSL(boost::asio::ssl::context::tlsv12);
  ssl_ctx->set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2);
      //| boost::asio::ssl::context::single_dh_use);
  ssl_ctx->use_certificate_chain_file(sk_f);
  ssl_ctx->use_private_key_file(sk_f, boost::asio::ssl::context::pem);
  ssl_ctx->use_tmp_dh_file("mt-core/secure/experimental/cpp/dh2048.pem");

  // impose cipher
  SSL_CTX_set_cipher_list(ssl_ctx->native_handle(), "DHE-RSA-AES256-SHA256");
  
  return ssl_ctx;
}

template class EndpointSSL<std::shared_ptr<google::protobuf::Message>>;
template class EndpointSSL<std::pair<Uri, std::shared_ptr<google::protobuf::Message>>>;
