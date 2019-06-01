#include "OutboundSearchConversationCreator.hpp"

#include "mt-core/tasks-base/src/cpp/TWorkerTask.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "threading/src/cpp/lib/StateMachineTask.hpp"
#include "mt-core/comms/src/cpp/OefEndpoint.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include "mt-core/comms/src/cpp/OutboundConversation.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "mt-core/conversations/src/cpp/ProtoPathMessageSender.hpp"
#include "mt-core/conversations/src/cpp/ProtoPathMessageReader.hpp"

class OutboundSearchConversation
  : public OutboundConversation
{
public:
  OutboundSearchConversation(
                             std::shared_ptr<OutboundSearchConnectorTask> connector,
                             std::size_t ident,
                             const Uri &uri,
                             std::shared_ptr<google::protobuf::Message> initiator)
    : OutboundConversation()
    , uri(uri)
    ,ident (ident)
  {
    this -> initiator = initiator;
  }

  static constexpr char const *LOGGING_NAME = "OutboundSearchConversation";
  Uri uri;
  std::shared_ptr<google::protobuf::Message> initiator;
  std::size_t ident;

  virtual std::size_t getAvailableReplyCount() const
  {
    return 0;
  }
  virtual std::shared_ptr<google::protobuf::Message> getReply(std::size_t replynumber)
  {
    return std::shared_ptr<google::protobuf::Message>();
  }
};

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
      std::cerr << "---> " << std::endl;
      uri.diagnostic();
      ep -> connect(uri, core);
      ep_send = std::make_shared<ProtoPathMessageSender>(ep);
      ep_read = std::make_shared<ProtoPathMessageReader>(ep);
      std::cout << " OutboundSearchConnectorTask  READ=" << ep_read.get() << std::endl;
      std::cout << " OutboundSearchConnectorTask  SEND=" << ep_send.get() << std::endl;

      ep_read -> onComplete = [this](bool success, int id, ConstCharArrayBuffer buffer){
      };

      ep -> writer = ep_send;
      ep -> reader = ep_read;
      ep -> go();
      std::cerr << "---> " << "connected" << std::endl;
      connected = true;
      worker -> makeRunnable();
    }
    catch(std::exception &ex)
    {
      std::cerr << "---> " << ex.what() << std::endl;
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
  FETCH_LOG_INFO(LOGGING_NAME, "Starting search conversation...");
  if (!searchConnector -> connected)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "no search conn");
    return NOT_COMPLETE;
  }

  FETCH_LOG_INFO(LOGGING_NAME, "Starting search conversation...");
  searchConnector -> ep_send -> send(workload -> ident, workload -> uri, workload -> initiator);
  FETCH_LOG_INFO(LOGGING_NAME, "Starting search ep send loop...");
  searchConnector -> ep -> run_sending();
  FETCH_LOG_INFO(LOGGING_NAME, "done..");
  return COMPLETE;
}




// , StateMachineTask<OutboundSearchConversationWorkerTask>>

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
  auto conv = std::make_shared<OutboundSearchConversation>(searchConnector, ident++, target_path, initiator);
  worker -> post(conv);
  return conv;
}
