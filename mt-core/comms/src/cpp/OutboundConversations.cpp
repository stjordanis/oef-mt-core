#include "OutboundConversations.hpp"

#include "mt-core/comms/src/cpp/IOutboundConversationCreator.hpp"

OutboundConversations::OutboundConversations()
{
}

OutboundConversations::~OutboundConversations()
{
}

void OutboundConversations::delConversationCreator(const std::string &target)
{
  creators.erase(target);
}

void OutboundConversations::addConversationCreator(const std::string &target, std::shared_ptr<IOutboundConversationCreator> creator)
{
  creators[target] = creator;
}

std::shared_ptr<OutboundConversation> OutboundConversations::startConversation(const std::string &target, std::shared_ptr<google::protobuf::Message> initiator)
{
  auto iter = creators.find(target);
  if (iter != creators.end())
  {
    return iter -> second -> start(initiator);
  }
  throw std::invalid_argument(target);
}

