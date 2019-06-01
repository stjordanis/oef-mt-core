#include "SearchConversationTask.hpp"

#include <utility>
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

SearchConversationTask::EntryPoint searchConversationTaskEntryPoints[] = {
  &SearchConversationTask::createConv,
  &SearchConversationTask::handleResponse,
};

SearchConversationTask::SearchConversationTask(
    std::shared_ptr<google::protobuf::Message> initiator,
    std::shared_ptr<OutboundConversations> outbounds
)
  :  StateMachineTask<SearchConversationTask>(this, searchConversationTaskEntryPoints)
  , initiator(initiator)
  , outbounds(outbounds)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task created.");
}

SearchConversationTask::~SearchConversationTask()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task gone.");
}

SearchConversationTask::StateResult SearchConversationTask::createConv(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Start");
  outbounds -> startConversation(
                                 Uri("outbound://search/update"),
                                 initiator
                                 );
  FETCH_LOG_INFO(LOGGING_NAME, "Sleeping");
  return SearchConversationTask::StateResult(1, DEFER);
}

SearchConversationTask::StateResult SearchConversationTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken");
  return SearchConversationTask::StateResult(0, COMPLETE);
}
