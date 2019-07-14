#include "EndpointSSL.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"

#include <openssl/ssl.h>
#include <cerrno> // for fopen
#include <cstdio> // for fopen
#include <memory>

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
          ERR_load_crypto_strings();
          // get peer pub key (agent id)
          if (X509* cert = SSL_get_peer_certificate(ssl_sock_p->native_handle()))
          {
            if(EVP_PKEY* pk = X509_get_pubkey(cert))
            {
              FETCH_LOG_ERROR(LOGGING_NAME, "Got Agent PubKey: ", to_string_EVP_PKEY(pk));
            }
          } 
          else
          {
            FETCH_LOG_WARN(LOGGING_NAME, "Couldn't get Agent public key from ssl socket : ", ERR_error_string(ERR_get_error(), NULL));
          }
          // get my pub key (core id)
          if (X509* cert = SSL_get_certificate(ssl_sock_p->native_handle()))
          {
            if(EVP_PKEY* pk = X509_get_pubkey(cert))
            {
              FETCH_LOG_ERROR(LOGGING_NAME, "Got Core PubKey: ", to_string_EVP_PKEY(pk));
              FETCH_LOG_ERROR(LOGGING_NAME, "Core PubKey: ", to_string_PEM_f(pk_f));
            }
          } 
          else
          {
            FETCH_LOG_WARN(LOGGING_NAME, "Couldn't get my public key from ssl socket");
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

std::string to_string_RSA(RSA* pk)
{
  std::string out{};

  BIO * mem = BIO_new(BIO_s_mem());
  // *** For base64 ***
  //BIO * b64f = BIO_new(BIO_f_base64());
  //mem = BIO_push(b64f, mem);
  //BIO_set_flags(mem, BIO_FLAGS_BASE64_NO_NL);
  //BIO_set_close(mem, BIO_CLOSE);
  // *** End base64 ***
  if (!mem) {
    FETCH_LOG_WARN("EndpointSSL", " while creating bio mem buf");
    return out;
  }
  int status = RSA_print(mem, pk, 0);
  if (!status) {
    FETCH_LOG_WARN("EndpointSSL", " while printing RSA key to bio");
    BIO_free(mem);
    return out;
  }
  char *pk_ptr = nullptr;
  unsigned long len = BIO_get_mem_data(mem, &pk_ptr);
  if (!len) {
    FETCH_LOG_WARN("EndpointSSL", " while getting bio char pointer");
    BIO_free(mem);
    return out;
  }
  std::string pk_str{pk_ptr,len};  
  out = pk_str;
  BIO_free(mem);

  return out;
}
  
std::string to_string_EVP_PKEY(EVP_PKEY* pk)
{
  std::string out{};

  int type = EVP_PKEY_base_id(pk); 
  switch (type) {
    // RSA
    case EVP_PKEY_RSA:
    case EVP_PKEY_RSA2:
    {
      RSA * rsa = EVP_PKEY_get1_RSA(pk);
      if (!rsa) {
        FETCH_LOG_WARN("EndpointSSL", " error getting rsa key");
        break;
      }
      out = to_string_RSA(rsa);
      RSA_free(rsa);
     /*
      BIO * mem = BIO_new(BIO_s_mem());
      // *** For base64 ***
      //BIO * b64f = BIO_new(BIO_f_base64());
      //mem = BIO_push(b64f, mem);
      //BIO_set_flags(mem, BIO_FLAGS_BASE64_NO_NL);
      //BIO_set_close(mem, BIO_CLOSE);
      // *** End base64 ***
      if (!mem) {
        FETCH_LOG_WARN("EndpointSSL", " while creating bio mem buf");
        RSA_free(rsa);
        break;
      }
      int status = RSA_print(mem, rsa, 0);
      if (!status) {
        FETCH_LOG_WARN("EndpointSSL", " while printing RSA key to bio");
        RSA_free(rsa);
        BIO_free(mem);
        break;
      }
      char *pk_ptr = nullptr;
      unsigned long len = BIO_get_mem_data(mem, &pk_ptr);
      if (!len) {
        FETCH_LOG_WARN("EndpointSSL", " while getting bio char pointer");
        RSA_free(rsa);
        BIO_free(mem);
        break;
      }
      std::string pk_str{pk_ptr,len};  
      RSA_free(rsa);
      BIO_free(mem);
      out = pk_str;
    */
      break;
   }

    // ECDSA TODO()

    // not supported
    default:
      break;
  }

  return out;
}

std::string to_string_PEM_f(const std::string file_path)
{
  std::string out{};

  FILE *fp = fopen(file_path.c_str(), "r");
  if (!fp) {
    FETCH_LOG_WARN("EndpointSSL", " while trying to open pub key pem file : ", strerror(errno));
    return out;
  }
  
  ERR_load_crypto_strings();
  EVP_PKEY* pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
  if(!pkey) {
    FETCH_LOG_WARN("EndpointSSL", " while trying to read EVP_PKEY from pem file : ", ERR_error_string(ERR_get_error(), NULL));
    fclose(fp);
    return out;
  }

  out = to_string_EVP_PKEY(pkey);
  fclose(fp);
  EVP_PKEY_free(pkey);
  return out;
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
  //ssl_ctx->set_password_callback(std::bind(&EndpointSSL::key_get_password, this));
  ssl_ctx->use_certificate_chain_file(sk_f);
  //ssl_ctx->use_private_key_file("mt-core/secure/experimental/cpp/server.pem", boost::asio::ssl::context::pem);
  //ssl_ctx->use_private_key_file("mt-core/secure/experimental/cpp/private_key_core.pem", boost::asio::ssl::context::pem);
  ssl_ctx->use_private_key_file(sk_f, boost::asio::ssl::context::pem);
  //ssl_ctx->use_rsa_private_key_file("mt-core/secure/experimental/cpp/private_key_core.pem", boost::asio::ssl::context::pem);
  ssl_ctx->use_tmp_dh_file("mt-core/secure/experimental/cpp/dh2048.pem");
  
  return ssl_ctx;
}

template class EndpointSSL<std::shared_ptr<google::protobuf::Message>>;
template class EndpointSSL<std::pair<Uri, std::shared_ptr<google::protobuf::Message>>>;
