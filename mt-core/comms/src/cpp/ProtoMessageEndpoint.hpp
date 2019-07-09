#pragma once

#include <memory>

#include "basic_comms/src/cpp/Endpoint.hpp"

class ProtoMessageReader;
class ProtoMessageSender;

#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/EndpointPipe.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageReader.hpp"

namespace google
{
  namespace protobuf
  {
    class Message;
  }
}

class ProtoMessageEndpoint
    : public EndpointPipe<EndpointBase<std::shared_ptr<google::protobuf::Message>>, google::protobuf::Message>
{
public:
  using EndpointType  = EndpointBase<std::shared_ptr<google::protobuf::Message>>;

  ProtoMessageEndpoint(std::shared_ptr<EndpointType> endpoint);

  virtual ~ProtoMessageEndpoint()
  {
  }

  void setEndianness(Endianness newstate);

  void setup(std::shared_ptr<ProtoMessageEndpoint>& myself);

  void setOnStartHandler(EndpointType::StartNotification handler)
  {
    endpoint -> onStart = std::move(handler);
  }

  void setOnCompleteHandler(ProtoMessageReader::CompleteNotification handler)
  {
    protoMessageReader->onComplete = std::move(handler);
  }

  void setOnErrorHandler(EndpointType::ErrorNotification handler)
  {
    endpoint->onError = std::move(handler);
  }

  void setOnEofHandler(EndpointType::EofNotification handler)
  {
    endpoint->onEof = std::move(handler);
  }

  void setOnProtoErrorHandler(EndpointType::ProtoErrorNotification handler)
  {
    endpoint->onProtoError = std::move(handler);
  }

protected:
  std::shared_ptr<ProtoMessageReader> protoMessageReader;
  std::shared_ptr<ProtoMessageSender> protoMessageSender;

private:
  ProtoMessageEndpoint(const ProtoMessageEndpoint &other) = delete;
  ProtoMessageEndpoint &operator=(const ProtoMessageEndpoint &other) = delete;
  bool operator==(const ProtoMessageEndpoint &other) = delete;
  bool operator<(const ProtoMessageEndpoint &other) = delete;
};
