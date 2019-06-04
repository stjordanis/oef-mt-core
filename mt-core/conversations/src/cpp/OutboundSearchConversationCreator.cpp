#include "OutboundSearchConversationCreator.hpp"

#include "mt-core/tasks-base/src/cpp/TWorkerTask.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "threading/src/cpp/lib/StateMachineTask.hpp"
#include "mt-core/comms/src/cpp/OefEndpoint.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "fetch_teams/ledger/logger.hpp"
#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"

#include "mt-core/conversations/src/cpp/ProtoPathMessageSender.hpp"
#include "mt-core/conversations/src/cpp/ProtoPathMessageReader.hpp"

#include "mt-core/conversations/src/cpp/SearchConversationTypes.hpp"

#include "protos/src/protos/search_message.pb.h"
#include "protos/src/protos/search_query.pb.h"
#include "protos/src/protos/search_remove.pb.h"
#include "protos/src/protos/search_response.pb.h"
#include "protos/src/protos/search_update.pb.h"
#include "protos/src/protos/search_transport.pb.h"

class OutboundSearchConversation
  : public OutboundConversation
{
public:
  OutboundSearchConversation(
                             SearchConversationType type,
                             std::shared_ptr<OutboundSearchConnectorTask> connector,
                             std::size_t ident,
                             const Uri &uri,
                             std::shared_ptr<google::protobuf::Message> initiator)
    : OutboundConversation()
    , uri(uri)
    , ident(ident)
    , type(type)
  {
    this -> initiator = initiator;
  }

  static constexpr char const *LOGGING_NAME = "OutboundSearchConversation";
  Uri uri;
  std::shared_ptr<google::protobuf::Message> initiator;
  std::size_t ident;
  std::size_t mcount = 0;
  SearchConversationType type;
};

template<class PROTOCLASS>
class OutboundTypedSearchConversation
  : public OutboundSearchConversation
{
public:
  OutboundTypedSearchConversation(
                             SearchConversationType type,
                             std::shared_ptr<OutboundSearchConnectorTask> connector,
                             std::size_t ident,
                             const Uri &uri,
                             std::shared_ptr<google::protobuf::Message> initiator)
    : OutboundSearchConversation(
                                 type,
                                 connector,
                                 ident,
                                 uri,
                                 initiator
                                 )
  {
  }

  std::vector<std::shared_ptr<PROTOCLASS>> responses;
  int status_code;
  std::string error_message;

  virtual void handleMessage(ConstCharArrayBuffer buffer) override
  {
    status_code = 0;
    auto r = std::make_shared<PROTOCLASS>();
    std::istream is(&buffer);
    if (!r -> ParseFromIstream(&is)){
      status_code = 91;
      error_message = "";
      wake();
      return;
    }
    responses.push_back(r);
    wake();
  }

  virtual void handleError(int status_code, const std::string& message) override
  {
    this -> status_code = status_code;
    error_message = message;
  }

  virtual std::size_t getAvailableReplyCount() const override
  {
    return responses.size();
  }
  virtual std::shared_ptr<google::protobuf::Message> getReply(std::size_t replynumber) override
  {
    return responses[replynumber];
  }

  virtual bool success() const override
  {
    return status_code == 0;
  }

  virtual int getErrorCode() const override
  {
    return status_code;
  }

  virtual const std::string& getErrorMessage() const override
  {
    return error_message;
  }

};

std::map<unsigned long, std::shared_ptr<OutboundSearchConversation>> ident2conversation;

class OutboundSearchConnectorTask;

// ------------------------------------------------------------------------------------------

class OutboundSearchConversationWorkerTask
  : public TWorkerTask<OutboundSearchConversation>
{
public:
  using Parent = TWorkerTask<OutboundSearchConversation>;
  using Workload = Parent::Workload;
  using WorkloadP = Parent::WorkloadP;
  using WorkloadProcessed = TWorkerTask<OutboundSearchConversation>::WorkloadProcessed;
  static constexpr char const *LOGGING_NAME = "OutboundSearchConversationWorkerTask";
  
  OutboundSearchConversationWorkerTask()
  {
  }
  
  virtual ~OutboundSearchConversationWorkerTask()
  {
  }

  virtual WorkloadProcessed process(WorkloadP workload, WorkloadState state) override;
  std::shared_ptr<OutboundSearchConnectorTask> searchConnector;
};

// ------------------------------------------------------------------------------------------

class OutboundSearchConnectorTask : public Task
{
public:
  static constexpr char const *LOGGING_NAME = "OutboundSearchConnectorTask";
  OutboundSearchConnectorTask(Core &core, const Uri &search_uri)
    : uri(search_uri)
    , core(core)
    , connected(false)
  {
    ep = std::make_shared<Endpoint>(core, 10000, 10000);
  }

  virtual ~OutboundSearchConnectorTask()
  {
  }

  virtual bool isRunnable(void) const
  {
    return true;
  }

  virtual ExitState run(void)
  {
    try {
      uri.diagnostic();
      ep -> connect(uri, core);
      ep_send = std::make_shared<ProtoPathMessageSender>(ep);
      ep_read = std::make_shared<ProtoPathMessageReader>(ep);
      FETCH_LOG_INFO(LOGGING_NAME,"READ=", ep_read.get());
      FETCH_LOG_INFO(LOGGING_NAME,"SEND=", ep_send.get());

      ep_read -> onComplete = [](bool success, unsigned long id, ConstCharArrayBuffer buffer){
        FETCH_LOG_INFO(LOGGING_NAME,"complete message ", id);

        auto iter = ident2conversation.find(id);

        if (iter != ident2conversation.end())
        {
          FETCH_LOG_INFO(LOGGING_NAME,"wakeup!!");
          iter -> second -> handleMessage(buffer);
        }
        else
        {
          FETCH_LOG_INFO(LOGGING_NAME,"complete message not handled");
        }
      };

      ep_read -> onError = [](unsigned long id, int status_code, const std::string& message) {
        FETCH_LOG_INFO(LOGGING_NAME,"error message ", id);
        auto iter = ident2conversation.find(id);

        if (iter != ident2conversation.end())
        {
          FETCH_LOG_INFO(LOGGING_NAME,"wakeup!!");
          iter -> second -> handleError(status_code, message);
        }
        else
        {
          FETCH_LOG_INFO(LOGGING_NAME,"complete message not handled");
        }
      };

      ep -> writer = ep_send;
      ep -> reader = ep_read;
      ep -> go();
      FETCH_LOG_WARN(LOGGING_NAME, "Connected");
      connected = true;
      worker -> makeRunnable();
    }
    catch(std::exception &ex)
    {
      FETCH_LOG_ERROR(LOGGING_NAME, ex.what());
    }
    return COMPLETE;
  }

  std::shared_ptr<OutboundSearchConversationWorkerTask> worker;

  std::shared_ptr<ProtoPathMessageSender> ep_send;
  std::shared_ptr<ProtoPathMessageReader> ep_read;
  std::shared_ptr<Endpoint> ep;
  Uri uri;
  Core &core;
  std::atomic<bool> connected;
};


// ------------------------------------------------------------------------------------------

OutboundSearchConversationWorkerTask::WorkloadProcessed OutboundSearchConversationWorkerTask::process(WorkloadP workload, WorkloadState state)
{
  FETCH_LOG_INFO(LOGGING_NAME, "process search conversation...");
  if (!searchConnector -> connected)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "no search conn");
    return NOT_COMPLETE;
  }

  FETCH_LOG_INFO(LOGGING_NAME, "Send initiator...");
  searchConnector -> ep_send -> send(workload -> ident, workload -> uri, workload -> initiator);
  FETCH_LOG_INFO(LOGGING_NAME, "Starting search ep send loop...");
  searchConnector -> ep -> run_sending();
  FETCH_LOG_INFO(LOGGING_NAME, "done..");
  return COMPLETE;
}

OutboundSearchConversationCreator::OutboundSearchConversationCreator(const Uri &search_uri, Core &core)
{
  endpoint = std::make_shared<ProtoMessageEndpoint>(core);

  worker = std::make_shared<OutboundSearchConversationWorkerTask>();
  searchConnector = std::make_shared<OutboundSearchConnectorTask>(core, search_uri);

  worker -> searchConnector = searchConnector;
  searchConnector -> worker = worker;

  worker -> submit();
  searchConnector -> submit();
}

OutboundSearchConversationCreator::~OutboundSearchConversationCreator()
{
  worker -> searchConnector.reset();
  searchConnector -> worker.reset();
  worker.reset();
  searchConnector.reset();
}

std::shared_ptr<OutboundConversation> OutboundSearchConversationCreator::start(const Uri &target_path, std::shared_ptr<google::protobuf::Message> initiator)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Starting search conversation...");
  auto this_id = ident++;

  std::shared_ptr<OutboundSearchConversation> conv;

  SearchConversationType type;
  if (target_path.path == "/update")
  {
    type = UPDATE;
    conv = std::make_shared<OutboundTypedSearchConversation<fetch::oef::pb::UpdateResponse>>(type, searchConnector, this_id, target_path, initiator);
  }
  else if (target_path.path == "/remove")
  {
    type = REMOVE;
    conv = std::make_shared<OutboundTypedSearchConversation<fetch::oef::pb::RemoveResponse>>(type, searchConnector, this_id, target_path, initiator);
  }
  else if (target_path.path == "/search")
  {
    type = UPDATE;
    conv = std::make_shared<OutboundTypedSearchConversation<fetch::oef::pb::SearchResponse>>(type, searchConnector, this_id, target_path, initiator);
  }
  else
  {
    throw std::invalid_argument(target_path.path + " is not a valid target, to start a OutboundSearchConversationCreator!");
  }

  ident2conversation[this_id] = conv;
  worker -> post(conv);
  return conv;
}
