#pragma once

#include <memory>
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "mt-core/comms/src/cpp/OefEndpoint.hpp"
#include "threading/src/cpp/lib/Notification.hpp"

class Core;
class IOefAgentTaskFactory;
class ProtoMessageReader;
class ProtoMessageSender;

class OefAgentEndpoint : public OefEndpoint
{
public:
  OefAgentEndpoint(Core &core);
  virtual ~OefAgentEndpoint();

  virtual void go();
  void setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory);
  void setup(std::shared_ptr<OefAgentEndpoint> myself);

  Notification::NotificationBuilder send(std::shared_ptr<google::protobuf::Message> s);
  void setEndianness(Endianness newstate);
protected:
private:
  mutable Mutex mutex;
  std::shared_ptr<IOefAgentTaskFactory> factory;

  std::shared_ptr<ProtoMessageReader> protoMessageReader;
  std::shared_ptr<ProtoMessageSender> protoMessageSender;

  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
