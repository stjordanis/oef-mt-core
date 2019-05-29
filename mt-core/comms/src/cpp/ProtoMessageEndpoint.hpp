#pragma once

#include <memory>

#include "basic_comms/src/cpp/Endpoint.hpp"

class ProtoMessageReader;
class ProtoMessageSender;

#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "basic_comms/src/cpp/Endpoint.hpp"
#include "threading/src/cpp/lib/Notification.hpp"

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class ProtoMessageEndpoint: public Endpoint
{
public:
  ProtoMessageEndpoint(Core &core);

  virtual ~ProtoMessageEndpoint()
  {
  }

  void setup(std::shared_ptr<ProtoMessageEndpoint> myself);
  void setEndianness(Endianness newstate);

  Notification::NotificationBuilder send(std::shared_ptr<google::protobuf::Message> s);
protected:
  std::shared_ptr<ProtoMessageReader> protoMessageReader;
  std::shared_ptr<ProtoMessageSender> protoMessageSender;
private:
  ProtoMessageEndpoint(const ProtoMessageEndpoint &other) = delete; // { copy(other); }
  ProtoMessageEndpoint &operator=(const ProtoMessageEndpoint &other) = delete; // { copy(other); return *this; }
  bool operator==(const ProtoMessageEndpoint &other) = delete; // const { return compare(other)==0; }
  bool operator<(const ProtoMessageEndpoint &other) = delete; // const { return compare(other)==-1; }
};
