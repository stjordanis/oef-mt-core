#pragma once

#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "fetch_teams/ledger/logger.hpp"

class Endpoint;

class ProtoPathMessageReader : public IMessageReader

{
public:
  using CompleteNotification = std::function<void (bool success, unsigned long id, ConstCharArrayBuffer buffer)>;
  using ErrorNotification = std::function<void(unsigned long id, int error_code, const std::string& message)>;

  static constexpr char const *LOGGING_NAME = "ProtoPathMessageReader";


  ProtoPathMessageReader(std::weak_ptr<Endpoint> endpoint)
  {
    this -> endpoint = endpoint;
  }
  virtual ~ProtoPathMessageReader()
  {
  }

  consumed_needed_pair initial();
  consumed_needed_pair checkForMessage(const buffers &data);

  CompleteNotification onComplete;
  ErrorNotification onError;
protected:
private:
  std::weak_ptr<Endpoint> endpoint;

  ProtoPathMessageReader(const ProtoPathMessageReader &other) = delete;
  ProtoPathMessageReader &operator=(const ProtoPathMessageReader &other) = delete;
  bool operator==(const ProtoPathMessageReader &other) = delete;
  bool operator<(const ProtoPathMessageReader &other) = delete;
};
