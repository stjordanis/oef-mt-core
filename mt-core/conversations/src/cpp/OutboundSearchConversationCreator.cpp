#include "OutboundSearchConversationCreator.hpp"

#include "mt-core/tasks/src/cpp/TWorkerTask.hpp"
#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "threading/src/cpp/lib/StateMachineTask.hpp"

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

// , StateMachineTask<OutboundSearchConversationWorkerTask>>

OutboundSearchConversationCreator::OutboundSearchConversationCreator(const std::string &search_uri, Core &core)
{
  endpoint = std::make_shared<ProtoMessageEndpoint>(core);
  worker = std::make_shared<OutboundSearchConversationWorkerTask>();
  worker -> submit();
}

OutboundSearchConversationCreator::~OutboundSearchConversationCreator()
{
}

std::shared_ptr<OutboundConversation> OutboundSearchConversationCreator::start(std::shared_ptr<google::protobuf::Message> initiator)
{
}
