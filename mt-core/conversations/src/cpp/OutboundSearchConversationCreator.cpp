#include "OutboundSearchConversationCreator.hpp"

#include "mt-core/tasks/src/cpp/TWorkerTask.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "threading/src/cpp/lib/StateMachineTask.hpp"
#include "mt-core/comms/src/cpp/OefEndpoint.hpp"


class OutboundSearchConversationWorkerTask
  : public TWorkerTask<OutboundConversation>
{
public:
  using Parent = TWorkerTask<OutboundConversation>;
  using Workload = Parent::Workload;
  using WorkloadP = Parent::WorkloadP;
  
  OutboundSearchConversationWorkerTask()
  {
  }
  virtual ~OutboundSearchConversationWorkerTask()
  {
  }

  virtual WorkloadProcessed process(WorkloadP workload, WorkloadState state) override
  {
    return COMPLETE;
  }
};

class OutboundSearchConnectorTask : public Task
{
public:
  OutboundSearchConnectorTask(Core &core, const Uri &search_uri)
    : uri(search_uri)
    , core(core)
  {
    ep = std::make_shared<OefEndpoint>(core);
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
      std::cerr << "---> " << "connected" << std::endl;
      
    }
    catch(std::exception &ex)
    {
      std::cerr << "---> " << ex.what() << std::endl;
    }
    return COMPLETE;
  }

  std::shared_ptr<OefEndpoint> ep;
  Uri uri;
  Core &core;
};


// , StateMachineTask<OutboundSearchConversationWorkerTask>>

OutboundSearchConversationCreator::OutboundSearchConversationCreator(const Uri &search_uri, Core &core)
{
  endpoint = std::make_shared<ProtoMessageEndpoint>(core);
  worker = std::make_shared<OutboundSearchConversationWorkerTask>();
  worker -> submit();

  searchConnector = std::make_shared<OutboundSearchConnectorTask>(core, search_uri);
  searchConnector -> submit();
}

OutboundSearchConversationCreator::~OutboundSearchConversationCreator()
{
}

std::shared_ptr<OutboundConversation> OutboundSearchConversationCreator::start(std::shared_ptr<google::protobuf::Message> initiator)
{
  
}
