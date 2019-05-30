#pragma once

#include <memory>
#include "mt-core/comms/src/cpp/IOutboundConversationCreator.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

class OutboundSearchConversationWorkerTask;
class ProtoMessageEndpoint;
class Core;
class OutboundSearchConnectorTask;

class OutboundSearchConversationCreator : public IOutboundConversationCreator
{
public:
  OutboundSearchConversationCreator(const Uri &search_uri, Core &core);
  virtual ~OutboundSearchConversationCreator();
  virtual std::shared_ptr<OutboundConversation> start(std::shared_ptr<google::protobuf::Message> initiator);
protected:
private:
  std::shared_ptr<OutboundSearchConversationWorkerTask> worker;
  std::shared_ptr<ProtoMessageEndpoint> endpoint;
  std::shared_ptr<OutboundSearchConnectorTask> searchConnector;

  OutboundSearchConversationCreator(const OutboundSearchConversationCreator &other) = delete;
  OutboundSearchConversationCreator &operator=(const OutboundSearchConversationCreator &other) = delete;
  bool operator==(const OutboundSearchConversationCreator &other) = delete;
  bool operator<(const OutboundSearchConversationCreator &other) = delete;
};
