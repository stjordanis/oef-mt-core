#include "SearchConversationTask.hpp"

#include <utility>
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
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
  auto this_sp = shared_from_this();
  std::weak_ptr<Task> this_wp = this_sp;
  FETCH_LOG_INFO(LOGGING_NAME, "Start");
  conversation = outbounds -> startConversation(
                                 Uri("outbound://search/update"),
                                 initiator
                                 );

  if (conversation -> makeNotification().Then( [this_wp](){ auto sp = this_wp.lock(); if (sp) { sp -> makeRunnable(); } } ).Waiting())
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Sleeping");
    return SearchConversationTask::StateResult(1, DEFER);
  }
  FETCH_LOG_INFO(LOGGING_NAME, "NOT Sleeping");
  return SearchConversationTask::StateResult(1, COMPLETE);
}

SearchConversationTask::StateResult SearchConversationTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken");
  FETCH_LOG_INFO(LOGGING_NAME, "MY GOD!! I AM COMPLETED!!");
  return SearchConversationTask::StateResult(0, COMPLETE);
}
