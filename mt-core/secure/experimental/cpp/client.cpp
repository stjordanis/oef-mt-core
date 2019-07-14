//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>

using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

enum { max_length = 1024 };

class client
{
public:
  client(boost::asio::io_context& io_context,
      boost::asio::ssl::context& context,
      const tcp::resolver::results_type& endpoints)
    : socket_(io_context, context)
  {
    socket_.set_verify_mode(boost::asio::ssl::verify_none);
    socket_.set_verify_callback(
        std::bind(&client::verify_certificate, this, _1, _2));

    connect(endpoints);
  }

private:
  bool verify_certificate(bool preverified,
      boost::asio::ssl::verify_context& ctx)
  {
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";

    return preverified;
  }

  void connect(const tcp::resolver::results_type& endpoints)
  {
    boost::asio::async_connect(socket_.lowest_layer(), endpoints,
        [this](const boost::system::error_code& error,
          const tcp::endpoint& /*endpoint*/)
        {
          if (!error)
          {
            handshake();
          }
          else
          {
            std::cout << "Connect failed: " << error.message() << "\n";
          }
        });
  }

  void handshake()
  {
    socket_.async_handshake(boost::asio::ssl::stream_base::client,
        [this](const boost::system::error_code& error)
        {
          if (!error)
          {
          ERR_load_crypto_strings();
          // get peer pub key (core id)
          if (X509* cert = SSL_get_peer_certificate(socket_.native_handle()))
          {
            if(EVP_PKEY* pk = X509_get_pubkey(cert))
            {
              RSA * rsa = EVP_PKEY_get1_RSA(pk);
              if (!rsa) {
                std::cout << "[sslclient] error getting rsa key" << std::endl;
                return;
              }
              std::cout << "[sslclient] getting rsa key" << std::endl;
              BIO * mem = BIO_new(BIO_s_mem());
              //BIO * b64f = BIO_new(BIO_f_base64());
              //mem = BIO_push(b64f, mem);
              //BIO_set_flags(mem, BIO_FLAGS_BASE64_NO_NL);
              //BIO_set_close(mem, BIO_CLOSE);
              if (!mem) {
                std::cout << "[sslclient] error creating bio mem buf" << std::endl;
                return;
              }
              std::cout << "[sslclient] creating bio mem buf" << std::endl;
              int status = RSA_print(mem, rsa, 0);
              if (!status) {
                std::cout << "[sslclient] error printing RSA key to bio" << std::endl;
                return;
              }
              std::cout << "[sslclient] printing RSA key to bio" << std::endl;
              char *pk_ptr = nullptr;
              long len = BIO_get_mem_data(mem, &pk_ptr);
              if (!len) {
                std::cout << "[sslclient] error getting bio char pointer" << std::endl;
                return;
              }
              std::cout << "[sslclient] getting bio char pointer" << std::endl;
              std::string pk_str(pk_ptr,len);
              std::cout << "[sslclient] Got Core PubKey: " << pk_str << std::endl;
            }
          } 
          else
          {
            std::cout << "[sslclient] Couldn't get Agent public key from ssl socket : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
          }
            send_request();
          }
          else
          {
            std::cout << "Handshake failed: " << error.message() << "\n";
          }
        });
  }

  void send_request()
  {
    std::cout << "Enter message: ";
    std::cin.getline(request_, max_length);
    size_t request_length = std::strlen(request_);

    boost::asio::async_write(socket_,
        boost::asio::buffer(request_, request_length),
        [this](const boost::system::error_code& error, std::size_t length)
        {
          if (!error)
          {
            receive_response(length);
          }
          else
          {
            std::cout << "Write failed: " << error.message() << "\n";
          }
        });
  }

  void receive_response(std::size_t length)
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(reply_, length),
        [this](const boost::system::error_code& error, std::size_t length)
        {
          if (!error)
          {
            std::cout << "Reply: ";
            std::cout.write(reply_, length);
            std::cout << "\n";
          }
          else
          {
            std::cout << "Read failed: " << error.message() << "\n";
          }
        });
  }

  boost::asio::ssl::stream<tcp::socket> socket_;
  char request_[max_length];
  char reply_[max_length];
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);

    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    //ctx.load_verify_file("mt-core/secure/experimental/cpp/ca.pem");
    //ctx.use_rsa_private_key_file("mt-core/secure/experimental/cpp/private_key.pem", boost::asio::ssl::context::pem);
    ctx.use_private_key_file("mt-core/secure/experimental/cpp/private_key.pem", boost::asio::ssl::context::pem);

    client c(io_context, ctx, endpoints);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
