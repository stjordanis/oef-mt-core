#pragma once

#include <memory>
#include "mt-core/comms/src/cpp/IOutboundConversationCreator.hpp"

class OutboundSearchConversationWorkerTask;
class ProtoMessageEndpoint;
class Core;

class OutboundSearchConversationCreator : public IOutboundConversationCreator
{
public:
  OutboundSearchConversationCreator(const std::string &search_uri, Core &core);
  virtual ~OutboundSearchConversationCreator();
  virtual std::shared_ptr<OutboundConversation> start(std::shared_ptr<google::protobuf::Message> initiator);
protected:
private:
  std::shared_ptr<OutboundSearchConversationWorkerTask> worker;
  std::shared_ptr<ProtoMessageEndpoint> endpoint;

  OutboundSearchConversationCreator(const OutboundSearchConversationCreator &other) = delete;
  OutboundSearchConversationCreator &operator=(const OutboundSearchConversationCreator &other) = delete;
  bool operator==(const OutboundSearchConversationCreator &other) = delete;
  bool operator<(const OutboundSearchConversationCreator &other) = delete;
};
