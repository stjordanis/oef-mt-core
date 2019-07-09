#pragma once

#include "mt-core/comms/src/cpp/IOefAgentTaskFactory.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "mt-core/agents/src/cpp/Agents.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "fetch_teams/ledger/experimental/crypto/ecdsa.hpp"

#include <openssl/ssl.h>


class InitialSecureHandshakeTaskFactory:public IOefAgentTaskFactory
{
public:
  static constexpr char const *LOGGING_NAME = "InitialSecureHandshakeTaskFactory";

  InitialSecureHandshakeTaskFactory(std::string core_key, std::shared_ptr<OefAgentEndpoint> the_endpoint,
      std::shared_ptr<OutboundConversations> outbounds, std::shared_ptr<Agents> agents)
    : IOefAgentTaskFactory(the_endpoint, outbounds)
  , agents_{std::move(agents)}
  , public_key_{""}
  , core_key_{std::move(core_key)}
  {
    // just to test openssl linkage
    SSL_load_error_strings();
    
    // just to test ledger/crypto
    using fetch::crypto::ECDSASigner;
    using fetch::byte_array::ToBase64;
    
    ECDSASigner signer;
    signer.GenerateKeys();

    std::cout << "Public Key...: " << ToBase64(signer.public_key()) << std::endl;
    std::cout << "Private Key..: " << ToBase64(signer.private_key()) << std::endl;
  }
  virtual ~InitialSecureHandshakeTaskFactory()
  {
  }

  virtual void processMessage(ConstCharArrayBuffer &data);
  // Process the message, throw exceptions if they're bad.

  virtual void endpointClosed(void) {}
protected:
private:

  InitialSecureHandshakeTaskFactory(const InitialSecureHandshakeTaskFactory &other) = delete;
  InitialSecureHandshakeTaskFactory &operator=(const InitialSecureHandshakeTaskFactory &other) = delete;
  bool operator==(const InitialSecureHandshakeTaskFactory &other) = delete;
  bool operator<(const InitialSecureHandshakeTaskFactory &other) = delete;

private:
  std::shared_ptr<Agents> agents_;
  std::string public_key_;
  std::string core_key_;
};
