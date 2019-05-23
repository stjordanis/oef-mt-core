#pragma once

#include <memory>

#include "basic_comms/src/cpp/Endpoint.hpp"

class Core;
class IOefAgentTaskFactory;
class ProtoMessageReader;
//class ProtoMessageSender;

class OefAgentEndpoint : public Endpoint
{
public:
  OefAgentEndpoint(Core &core);
  virtual ~OefAgentEndpoint();

  virtual void go();
  void setFactory(std::shared_ptr<IOefAgentTaskFactory> new_factory);
protected:
private:
  mutable Mutex mutex;
  std::shared_ptr<IOefAgentTaskFactory> factory;

  std::shared_ptr<ProtoMessageReader> protoMessageReader;
  //std::shared_ptr<ProtoMessageSender> protoMessageSender;

  OefAgentEndpoint(const OefAgentEndpoint &other) = delete;
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete;
  bool operator==(const OefAgentEndpoint &other) = delete;
  bool operator<(const OefAgentEndpoint &other) = delete;
};
