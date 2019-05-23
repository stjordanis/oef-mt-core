#pragma once

#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"

class ProtoMessageReader:public IMessageReader

{
public:
  using CompleteNotification = std::function<void (ConstCharArrayBuffer buffer)>;

  ProtoMessageReader()
  {
  }
  virtual ~ProtoMessageReader()
  {
  }

  consumed_needed_pair initial();
  consumed_needed_pair checkForMessage(const buffers &data);

  CompleteNotification onComplete;
protected:
private:
  ProtoMessageReader(const ProtoMessageReader &other) = delete;
  ProtoMessageReader &operator=(const ProtoMessageReader &other) = delete;
  bool operator==(const ProtoMessageReader &other) = delete;
  bool operator<(const ProtoMessageReader &other) = delete;
};
