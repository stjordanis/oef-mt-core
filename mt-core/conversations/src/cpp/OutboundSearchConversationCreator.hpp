#pragma once

#include <memory>
#include <map>

#include "mt-core/comms/src/cpp/IOutboundConversationCreator.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"

class OutboundSearchConversationWorkerTask;
class ProtoMessageEndpoint;
class Core;
class OutboundSearchConnectorTask;

class OutboundSearchConversationCreator : public IOutboundConversationCreator
{
public:
  OutboundSearchConversationCreator(const std::string &core_key, const Uri &core_uri,
      const Uri &search_uri, Core &core, std::shared_ptr<OutboundConversations> outbounds);
  virtual ~OutboundSearchConversationCreator();
  virtual std::shared_ptr<OutboundConversation> start(const Uri &target_path, std::shared_ptr<google::protobuf::Message> initiator);
protected:
private:
  static constexpr char const *LOGGING_NAME = "OutboundSearchConversationCreator";

  std::size_t ident = 1;

  std::shared_ptr<OutboundSearchConversationWorkerTask> worker;
  std::shared_ptr<ProtoMessageEndpoint> endpoint;
  std::shared_ptr<OutboundSearchConnectorTask> searchConnector;

  OutboundSearchConversationCreator(const OutboundSearchConversationCreator &other) = delete;
  OutboundSearchConversationCreator &operator=(const OutboundSearchConversationCreator &other) = delete;
  bool operator==(const OutboundSearchConversationCreator &other) = delete;
  bool operator<(const OutboundSearchConversationCreator &other) = delete;
};
