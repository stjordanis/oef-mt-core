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

std::string to_string_RSA(RSA* pk)
{
  std::string out{};

  BIO * mem = BIO_new(BIO_s_mem());
  // *** For base64 ***
  // TOFIX doesn't output same text as when ASCII priting file
  //BIO * b64f = BIO_new(BIO_f_base64());
  //mem = BIO_push(b64f, mem);
  //BIO_set_flags(mem, BIO_FLAGS_BASE64_NO_NL);
  //BIO_set_close(mem, BIO_CLOSE);
  // *** End base64 ***
  if (!mem) {
    std::cout << "[sslclient] while creating bio mem buf" << std::endl;
    return out;
  }
  int status = RSA_print(mem, pk, 0);
  if (!status) {
    std::cout << "[sslclient] while printing RSA key to bio" << std::endl;
    BIO_free(mem);
    return out;
  }
  char *pk_ptr = nullptr;
  unsigned long len = BIO_get_mem_data(mem, &pk_ptr);
  if (!len) {
    std::cout << "[sslclient] while getting bio char pointer" << std::endl;
    BIO_free(mem);
    return out;
  }
  //std::string pk_str{pk_ptr,len};  
  const unsigned int PREFIX=35;
  const unsigned int SUFFIX=27;
  std::string pk_str{pk_ptr+PREFIX,len-PREFIX-SUFFIX}; // parse Modulus only
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
        std::cout << "[sslclient] error getting rsa key" << std::endl;
        break;
      }
      out = to_string_RSA(rsa);
      RSA_free(rsa);
      break;
   }

    // ECDSA TODO()

    // not supported
    default:
      break;
  }

  return out;
}

EVP_PKEY* from_PEM_f(const std::string file_path)
{
  FILE *fp = fopen(file_path.c_str(), "r");
  if (!fp) {
    std::cout << "[sslclient] while trying to open pub key pem file : " << strerror(errno) << std::endl;
    return NULL;
  }
  
  ERR_load_crypto_strings();
  EVP_PKEY* pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
  if(!pkey) {
    std::cout << "[sslclient] while trying to read EVP_PKEY from pem file : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
    fclose(fp);
    return NULL;
  }

  return pkey;
}

std::string to_string_PEM_f(const std::string file_path)
{
  std::string out{};

  FILE *fp = fopen(file_path.c_str(), "r");
  if (!fp) {
    std::cout << "[sslclient] while trying to open pub key pem file : " << strerror(errno) << std::endl;
    return out;
  }
  
  ERR_load_crypto_strings();
  EVP_PKEY* pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
  if(!pkey) {
    std::cout << "[sslclient] while trying to read EVP_PKEY from pem file : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
    fclose(fp);
    return out;
  }

  out = to_string_EVP_PKEY(pkey);
  fclose(fp);
  EVP_PKEY_free(pkey);
  return out;
}

bool are_equal(EVP_PKEY* a, EVP_PKEY* b)
{
  return EVP_PKEY_cmp(a,b) == 1;
}

class client
{
public:
  client(boost::asio::io_context& io_context,
      boost::asio::ssl::context& context,
      const tcp::resolver::results_type& endpoints,
      std::string core_key_file)
    : socket_(io_context, context),
      core_key_file_(core_key_file)
  {
    socket_.set_verify_mode(boost::asio::ssl::verify_none);
    socket_.set_verify_callback(
        std::bind(&client::verify_certificate, this, _1, _2));

    EVP_PKEY* core_id = from_PEM_f(core_key_file_);
    if (core_id) {
      std::cout << "[sslclient] Core ID: " << to_string_EVP_PKEY(core_id) << std::endl;
      //RSA * rsa = EVP_PKEY_get1_RSA(core_id);
      //unsigned char* id_b64;
      //i2d_RSAPublicKey(rsa,&id_b64);
      //std::cout << "[sslclient] Core ID:" << id_b64 << std::endl;
      EVP_PKEY_free(core_id);
    }

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

    //return preverified;
    return true;
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
          ERR_load_crypto_strings(); // TOFIX doesn't print useful info
          // get peer pub key (core id)
          if (X509* cert = SSL_get_peer_certificate(socket_.native_handle()))
          {
            if(EVP_PKEY* pk = X509_get_pubkey(cert))
            {
              std::cout << "[sslclient] Got Core PubKey: " << to_string_EVP_PKEY(pk) << std::endl;
              EVP_PKEY* core_id = from_PEM_f(core_key_file_);
              if (core_id) {
                std::cout << "[sslclient] -> How does it compare to Core id? " << are_equal(pk,core_id) << std::endl;
                EVP_PKEY_free(core_id);
              }
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
    //std::cin.getline(request_, max_length);
    strcpy(request_,"I am an ssl client");
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

  std::string core_key_file_;

};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 5)
    {
      std::cerr << "Usage: client <keyfile> <host> <port> <coreKeyfile>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[2], argv[3]);

    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);
    ctx.use_certificate_chain_file(argv[1]);
    ctx.use_private_key_file(argv[1], boost::asio::ssl::context::pem);
    // Enforced by ssl server (core)
    //SSL_CTX_set_cipher_list(ctx.native_handle(), "ECDH-RSA-AES256-SHA384");
    
    client c(io_context, ctx, endpoints, argv[4]);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
