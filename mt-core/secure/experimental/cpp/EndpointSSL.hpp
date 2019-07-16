#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/secure/experimental/cpp/public_key_utils.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/evp.h>


template <typename TXType>
class EndpointSSL
  : public Endpoint<TXType>
  , public std::enable_shared_from_this<EndpointSSL<TXType>>
{
public:
  using Endpoint<TXType>::state;
  using Endpoint<TXType>::readBuffer;
  using Endpoint<TXType>::sendBuffer;
  using std::enable_shared_from_this<EndpointSSL<TXType>>::shared_from_this;

  using Socket = typename Endpoint<TXType>::Socket;
  using SocketSSL = boost::asio::ssl::stream<Socket>;
  using ContextSSL = boost::asio::ssl::context;
  using Lock      = typename Endpoint<TXType>::Lock;

  static constexpr char const *LOGGING_NAME = "EndpointSSL";

  EndpointSSL(const EndpointSSL &other) = delete;
  EndpointSSL &operator=(const EndpointSSL &other) = delete;
  bool operator==(const EndpointSSL &other) = delete;
  bool operator<(const EndpointSSL &other) = delete;

  EndpointSSL(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
      ,std::string sk_file = "mt-core/secure/experimental/cpp/core.pem"
      ,std::string pk_file = "mt-core/secure/experimental/cpp/core_pub.pem" // not mandatory, for debug
  );

  virtual ~EndpointSSL();

  virtual Socket& socket() override
  {
    if (ssl_setup) 
      return ssl_sock_p->next_layer();
    else 
      return sock;
  }

  virtual void close() override;
  virtual void go() override;

  std::string agent_key()
  {
    if(*state == this->RUNNING_ENDPOINT)
      return agent_key_;
    else
      return "";
  }
  
  std::shared_ptr<EvpPublicKey> get_peer_ssl_key();

protected:
  virtual void async_read(const std::size_t& bytes_needed) override;
  virtual void async_write() override;

private:
  SocketSSL* ssl_sock_p;
  ContextSSL* ssl_ctx_p;
  std::string key_;
  bool ssl_setup;

  Socket sock;

  ContextSSL* make_ssl_ctx();

  std::string agent_key_;
  std::string agent_ssl_key_;

  std::string sk_f;
  std::string pk_f; // not mandatory, for debug

};
  
bool verify_agent_certificate(bool, boost::asio::ssl::verify_context&);
