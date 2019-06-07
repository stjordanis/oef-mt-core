#include "mt-core/conversations/src/cpp/SearchQueryTask.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "protos/src/protos/search_response.pb.h"
#include "mt-core/tasks/src/cpp/utils.hpp"


SearchQueryTask::EntryPoint searchQueryTaskEntryPoints[] = {
    &SearchQueryTask::createConv,
    &SearchQueryTask::handleResponse,
};

SearchQueryTask::SearchQueryTask(
    std::shared_ptr<SearchQueryTask::IN_PROTO> initiator,
    std::shared_ptr<OutboundConversations> outbounds,
    std::shared_ptr<OefAgentEndpoint> endpoint,
    uint32_t msg_id,
    std::string core_key,
    std::string agent_uri,
    uint16_t ttl)
    :  SearchConverstationTask(
        "search",
        std::move(initiator),
        std::move(outbounds),
        std::move(endpoint),
        msg_id,
        std::move(core_key),
        std::move(agent_uri),
        searchQueryTaskEntryPoints,
        this)
    , ttl_{ttl}
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task created.");
}

SearchQueryTask::~SearchQueryTask()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Task gone.");
}

SearchQueryTask::StateResult SearchQueryTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken ");
  FETCH_LOG_INFO(LOGGING_NAME, "Response.. ",
                 conversation -> getAvailableReplyCount()
  );

  if (!conversation->success())
  {
    FETCH_LOG_WARN(LOGGING_NAME, "Search call returned error...");
    return SearchQueryTask::StateResult(0, ERRORED);
  }

  if (conversation->getAvailableReplyCount()==0)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "No available reply for search query, waiting more...");
    return SearchQueryTask::StateResult(0, DEFER);
  }

  auto response = std::static_pointer_cast<fetch::oef::pb::SearchResponse>(conversation->getReply(0));

  FETCH_LOG_WARN(LOGGING_NAME, "Got response proto: ", response->DebugString(),
      " , reply size=", conversation->getAvailableReplyCount());

  auto answer = std::make_shared<OUT_PROTO>();

  answer->set_answer_id(msg_id_);

  if (ttl_ == 1)
  {
    FETCH_LOG_INFO(LOGGING_NAME,  "Got search response: ", response->DebugString(), ", size: ", response->result_size());
    if (response->result_size() < 1)
    {
      FETCH_LOG_WARN(LOGGING_NAME, "Got empty search result!");
    }
    else
    {
      auto answer_agents = answer->mutable_agents();
      for (auto &item : response->result())
      {
        auto agts = item.agents();
        for (auto &a : agts)
        {
          OEFURI::URI uri;
          uri.parseAgent(a.key());
          answer_agents->add_agents(uri.agentPartAsString());
        }
      }
      FETCH_LOG_INFO(LOGGING_NAME, "Sending ", answer_agents->agents().size(), "agents to ", agent_uri_);
    }
  }
  else
  {
    FETCH_LOG_INFO(LOGGING_NAME,  "Got wide search response: ", response->DebugString());
    if (response->result_size()<1)
    {
      FETCH_LOG_WARN(LOGGING_NAME, "Got empty search result!");
    }
    else
    {
      auto agents_wide = answer->mutable_agents_wide();
      int agents_nbr = 0;
      for (auto &item : response->result())
      {
        auto *aw_item = agents_wide->add_result();
        aw_item->set_key(item.key());
        aw_item->set_ip(item.ip());
        aw_item->set_port(item.port());
        aw_item->set_info(item.info());
        aw_item->set_distance(item.distance());
        auto agts = item.agents();
        agents_nbr += item.agents().size();
        for (auto &a : agts)
        {
          auto *aw = aw_item->add_agents();
          aw->set_key(a.key());
          aw->set_score(a.score());
        }
      }
      FETCH_LOG_INFO(LOGGING_NAME, "Sending ", agents_nbr, "agents to ", agent_uri_);
    }
  }


  if (sendReply)
  {
    sendReply(answer, endpoint);
  }
  else
  {
    FETCH_LOG_WARN(LOGGING_NAME, "No sendReply!!");
  }

  FETCH_LOG_INFO(LOGGING_NAME, "COMPLETE");

  return SearchQueryTask::StateResult(0, COMPLETE);
}

std::shared_ptr<SearchQueryTask::REQUEST_PROTO> SearchQueryTask::make_request_proto()
{
  auto search_query = std::make_shared<fetch::oef::pb::SearchQuery>();
  search_query->set_source_key(core_key_);
  search_query->mutable_model()->CopyFrom(initiator->query());
  search_query->set_ttl(ttl_);
  FETCH_LOG_INFO(LOGGING_NAME, "Sending query to search: ", search_query->DebugString());
  return search_query;
}