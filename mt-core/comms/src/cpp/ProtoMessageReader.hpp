#pragma once

#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"

class OefAgentEndpoint;

class ProtoMessageReader : public IMessageReader

{
public:
  using CompleteNotification = std::function<void (ConstCharArrayBuffer buffer)>;

  ProtoMessageReader(std::weak_ptr<OefAgentEndpoint> &endpoint)
  {
    this -> endpoint = endpoint;
  }
  virtual ~ProtoMessageReader()
  {
  }

  consumed_needed_pair initial();
  consumed_needed_pair checkForMessage(const buffers &data);

  CompleteNotification onComplete;

  void setEndianness(Endianness newstate);
protected:
private:
  std::weak_ptr<OefAgentEndpoint> endpoint;

  void setDetectedEndianness(Endianness newstate);

  Endianness endianness = DUNNO;

  ProtoMessageReader(const ProtoMessageReader &other) = delete;
  ProtoMessageReader &operator=(const ProtoMessageReader &other) = delete;
  bool operator==(const ProtoMessageReader &other) = delete;
  bool operator<(const ProtoMessageReader &other) = delete;
};
