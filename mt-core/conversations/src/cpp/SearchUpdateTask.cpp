#include "mt-core/conversations/src/cpp/SearchUpdateTask.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "protos/src/protos/search_response.pb.h"
#include "monitoring/src/cpp/lib/Counter.hpp"

static Counter update_task_created("mt-core.search.update.tasks_created");
static Counter update_task_errored("mt-core.search.update.tasks_errored");
static Counter update_task_succeeded("mt-core.search.update.tasks_succeeded");

SearchUpdateTask::EntryPoint searchUpdateTaskEntryPoints[] = {
    &SearchUpdateTask::createConv,
    &SearchUpdateTask::handleResponse,
};

SearchUpdateTask::SearchUpdateTask(
    std::shared_ptr<SearchUpdateTask::IN_PROTO> initiator,
    std::shared_ptr<OutboundConversations> outbounds,
    std::shared_ptr<OefAgentEndpoint> endpoint,
    uint32_t msg_id,
    std::string core_key,
    std::string agent_uri)
    :  SearchConversationTask(
        "update",
        std::move(initiator),
        std::move(outbounds),
        std::move(endpoint),
        msg_id,
        std::move(core_key),
        std::move(agent_uri),
        searchUpdateTaskEntryPoints,
        this)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task created.");
  update_task_created++;
}

SearchUpdateTask::~SearchUpdateTask()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task gone.");
}

SearchUpdateTask::StateResult SearchUpdateTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken ");
  FETCH_LOG_INFO(LOGGING_NAME, "Response.. ",
                 conversation -> getAvailableReplyCount()
  );

  if (conversation -> getAvailableReplyCount() == 0){
    update_task_errored++;
    return SearchUpdateTask::StateResult(0, ERRORED);
  }

  auto resp = conversation->getReply(0);
  if (!resp){
    FETCH_LOG_ERROR(LOGGING_NAME, "Got nullptr as reply");
    update_task_errored++;
    return SearchUpdateTask::StateResult(0, ERRORED);
  }
  auto response = std::static_pointer_cast<fetch::oef::pb::UpdateResponse>(resp);

  // TODO should add a status answer, even in the case of no error

  if (response->status() != fetch::oef::pb::UpdateResponse_ResponseType_SUCCESS)
  {
    auto answer = std::make_shared<OUT_PROTO>();
    answer->set_answer_id(msg_id_);
    auto error = answer->mutable_oef_error();
    error->set_operation(fetch::oef::pb::Server_AgentMessage_OEFError::REGISTER_SERVICE);
    FETCH_LOG_WARN(LOGGING_NAME, "Sending error ", error, " ", agent_uri_);
    update_task_errored++;
    if (sendReply)
    {
      sendReply(answer, endpoint);
    }
    else
    {
      FETCH_LOG_WARN(LOGGING_NAME, "No sendReply!!");
    }
  }
  else
  {
    update_task_succeeded++;
  }

  FETCH_LOG_INFO(LOGGING_NAME, "COMPLETE");

  return SearchUpdateTask::StateResult(0, COMPLETE);
}

std::shared_ptr<SearchUpdateTask::REQUEST_PROTO> SearchUpdateTask::make_request_proto()
{
  auto update = std::make_shared<fetch::oef::pb::Update>();
  update -> set_key(core_key_);
  fetch::oef::pb::Update_DataModelInstance* dm = update -> add_data_models();
  dm->set_key(agent_uri_);
  dm->mutable_model()->CopyFrom(initiator->description().model());
  dm->mutable_values()->CopyFrom(initiator->description().values());

  return update;
}
