#include "SearchConversationTask.hpp"

#include <utility>
#include "fetch_teams/ledger/logger.hpp"
#include "mt-core/comms/src/cpp/OutboundConversations.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

#include "protos/src/protos/agent.pb.h"

SearchConversationTask::EntryPoint searchConversationTaskEntryPoints[] = {
  &SearchConversationTask::createConv,
  &SearchConversationTask::handleResponse,
};

SearchConversationTask::SearchConversationTask(
    SearchConversationType type,
    std::shared_ptr<google::protobuf::Message> initiator,
    std::shared_ptr<OutboundConversations> outbounds,
    std::shared_ptr<OefAgentEndpoint> endpoint
)
  :  StateMachineTask<SearchConversationTask>(this, searchConversationTaskEntryPoints)
  , initiator(initiator)
  , outbounds(outbounds)
  , type(type)
  , endpoint(endpoint)
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


std::shared_ptr<google::protobuf::Message> SearchConversationTask::createUpdateApiResponse(std::shared_ptr<google::protobuf::Message> search_response)
{
  auto resp = std::make_shared<fetch::oef::pb::Server>();

  std::shared_ptr<google::protobuf::Message> r = resp;
  return r;
}

std::shared_ptr<google::protobuf::Message> SearchConversationTask::createRemoveApiResponse(std::shared_ptr<google::protobuf::Message> search_response)
{
  auto resp = std::make_shared<fetch::oef::pb::Server>();
  std::shared_ptr<google::protobuf::Message> r = resp;
  return r;
}

std::shared_ptr<google::protobuf::Message> SearchConversationTask::createWideSearchApiResponse(std::shared_ptr<google::protobuf::Message> search_response)
{
  auto resp = std::make_shared<fetch::oef::pb::Server>();
  std::shared_ptr<google::protobuf::Message> r = resp;
  return r;
}

std::shared_ptr<google::protobuf::Message> SearchConversationTask::createSearchApiResponse(std::shared_ptr<google::protobuf::Message> search_response)
{
  auto resp = std::make_shared<fetch::oef::pb::Server>();
  std::shared_ptr<google::protobuf::Message> r = resp;
  return r;
}

/*
  // get payload 
  if(msg_operation == "update") {
    auto update_resp = pbs::deserialize<pb::UpdateResponse>(*payload);
    logger.debug("::process_message_ received update confirmation for msg {} (aka {})  : {} ",
        smsg_id, amsg_id, pbs::to_string(update_resp));
    msg_continuation(ec, OefSearchResponse{});
    return;
  } else

  if(msg_operation == "remove") {
    auto remove_resp = pbs::deserialize<pb::RemoveResponse>(*payload);
    logger.debug("::process_message_ received remove confirmation for msg {} (aka {}) : {} ", 
        smsg_id, amsg_id, pbs::to_string(remove_resp));
    msg_continuation(ec, OefSearchResponse{});
    return;
  } else
  
  if(msg_operation == "search-local") {
    auto search_resp = pbs::deserialize<pb::SearchResponse>(*payload);
    logger.debug("::process_message_ received local search results for msg {} (aka {}) : {} ", 
        smsg_id, amsg_id, pbs::to_string(search_resp));
    // get agents
    std::vector<std::string> agents{};
    auto items = search_resp.result();
    for (auto& item : items) {
      auto agts = item.agents();
      for (auto& a : agts) {
        std::string key{a.key()};
        agents.emplace_back(key);
      }
    }
    msg_continuation(ec, OefSearchResponse{agents});
    return;
  } else
  
  if(msg_operation == "search-wide") {
    auto search_resp = pbs::deserialize<pb::SearchResponse>(*payload);
    logger.debug("::process_message_ received wide search results for msg {} (aka {}) : {} ", 
        smsg_id, amsg_id, pbs::to_string(search_resp));
    // get SearchResultWide
    pb::Server_SearchResultWide agents_wide;
    auto items = search_resp.result();
    for (auto& item : items) {
      auto* aw_item = agents_wide.add_result();
      aw_item->set_key(item.key());
      aw_item->set_ip(item.ip());
      aw_item->set_port(item.port());
      aw_item->set_info(item.info());
      aw_item->set_distance(item.distance());
      auto agts = item.agents();
      for (auto& a : agts) {
        auto *aw = aw_item->add_agents();
        aw->set_key(a.key());
        aw->set_score(a.score());
      }
    }
    msg_continuation(ec, OefSearchResponse{agents_wide});
    return;
  } 
*/



SearchConversationTask::StateResult SearchConversationTask::handleResponse(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Woken ", type);
  FETCH_LOG_INFO(LOGGING_NAME, "Response.. ",
                 conversation -> getAvailableReplyCount()
                 );
  FETCH_LOG_INFO(LOGGING_NAME, "MY GOD!! I AM COMPLETED!!");

  std::shared_ptr<google::protobuf::Message> response;

  switch(type)
  {
  case SEARCH_NARROW:
    FETCH_LOG_INFO(LOGGING_NAME, "createSearchApiResponse");
    response = createSearchApiResponse    (conversation -> getReply(0));
    break;
  case SEARCH_WIDE:
    FETCH_LOG_INFO(LOGGING_NAME, "createWideSearchApiResponse");
    response = createWideSearchApiResponse(conversation -> getReply(0));
    break;
  case UPDATE:
    FETCH_LOG_INFO(LOGGING_NAME, "createUpdateApiResponse");
    response = createUpdateApiResponse    (conversation -> getReply(0));
    break;
  case REMOVE:
    FETCH_LOG_INFO(LOGGING_NAME, "createRemoveApiResponse");
    response = createRemoveApiResponse    (conversation -> getReply(0));
    break;
  }

  if (sendReply)
  {
    sendReply(response, endpoint);
  }
  else
  {
    FETCH_LOG_WARN(LOGGING_NAME, "No sendReply!!");
  }
  
  FETCH_LOG_INFO(LOGGING_NAME, "COMPLETE");

  return SearchConversationTask::StateResult(0, COMPLETE);
}
