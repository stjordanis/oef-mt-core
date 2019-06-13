#include "mt-core/conversations/src/cpp/SearchRemoveTask.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "protos/src/protos/search_response.pb.h"


SearchRemoveTask::EntryPoint searchRemoveTaskEntryPoints[] = {
    &SearchRemoveTask::createConv,
    &SearchRemoveTask::handleResponse,
};

SearchRemoveTask::SearchRemoveTask(
    std::shared_ptr<SearchRemoveTask::IN_PROTO> initiator,
    std::shared_ptr<OutboundConversations> outbounds,
    std::shared_ptr<OefAgentEndpoint> endpoint,
    uint32_t msg_id,
    std::string core_key,
    std::string agent_uri,
    bool remove_row)
    :  SearchConversationTask(
        "remove",
        std::move(initiator),
        std::move(outbounds),
        std::move(endpoint),
        msg_id,
        std::move(core_key),
        std::move(agent_uri),
        searchRemoveTaskEntryPoints,
        this)
    , remove_row_(remove_row)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task created.");
}

SearchRemoveTask::~SearchRemoveTask()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task gone.");
}

SearchRemoveTask::StateResult SearchRemoveTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken ");
  FETCH_LOG_INFO(LOGGING_NAME, "Response.. ",
                 conversation -> getAvailableReplyCount()
  );

  auto response = std::static_pointer_cast<fetch::oef::pb::RemoveResponse>(conversation->getReply(0));

  // TODO should add a status answer, even in the case of no error

  if (response->status() != fetch::oef::pb::RemoveResponse_ResponseType_SUCCESS)
  {
    std::shared_ptr<OUT_PROTO> answer = std::make_shared<OUT_PROTO>();
    answer->set_answer_id(msg_id_);
    auto error = answer->mutable_oef_error();
    error->set_operation(fetch::oef::pb::Server_AgentMessage_OEFError::UNREGISTER_SERVICE);
    FETCH_LOG_WARN(LOGGING_NAME, "Sending error ", error, " to ", agent_uri_);

    if (sendReply)
    {
      sendReply(answer, endpoint);
    }
    else
    {
      FETCH_LOG_WARN(LOGGING_NAME, "No sendReply!!");
    }
  }

  FETCH_LOG_INFO(LOGGING_NAME, "COMPLETE");

  return SearchRemoveTask::StateResult(0, COMPLETE);
}

std::shared_ptr<SearchRemoveTask::REQUEST_PROTO> SearchRemoveTask::make_request_proto()
{
  auto remove = std::make_shared<fetch::oef::pb::Remove>();
  remove->set_key(core_key_);
  remove->set_all(remove_row_);
  if (remove_row_)
  {
    remove->set_agent_key(agent_uri_);
  }
  if (initiator)
  {
    auto dmi = remove->add_data_models();
    dmi->set_key(agent_uri_);
    dmi->mutable_model()->CopyFrom(initiator->description().model());
    dmi->mutable_values()->CopyFrom(initiator->description().values());
  }
  return remove;
}
