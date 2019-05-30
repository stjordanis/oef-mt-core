#include "MtCore.hpp"

#include <iostream>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/tasks/src/cpp/InitialHandshakeTaskFactory.hpp"

using namespace std::placeholders;

int MtCore::run(const MtCore::args &args)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Starting core...");
  listeners = std::make_shared<OefListenerSet>();
  core = std::make_shared<Core>();
  auto tasks = std::make_shared<Taskpool>();
  tasks -> setDefault();

  std::function<void (void)> run_comms = std::bind(&Core::run, core.get());
  std::function<void (std::size_t thread_number)> run_tasks = std::bind(&Taskpool::run, tasks.get(), _1);

  FETCH_LOG_INFO(LOGGING_NAME, "comms_thread_count ", args.comms_thread_count);
  FETCH_LOG_INFO(LOGGING_NAME, "tasks_thread_count ", args.tasks_thread_count);

  comms_runners.start(args.comms_thread_count, run_comms);
  tasks_runners.start(args.tasks_thread_count, run_tasks);

  //outbounds -> addConversationCreator("search", std::make_shared<OutboundSearchConversationCreator>(args.search_uri, *core));
  agents_ = std::make_shared<Agents>();

  startListeners(args.listen_ports);

  while(1)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "ok");
    sleep(10);
  }
  return 0;
}

void MtCore::startListeners(const std::vector<int> &ports)
{
  IOefListener::FactoryCreator initialFactoryCreator =
    [this](std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      return std::make_shared<InitialHandshakeTaskFactory>(endpoint, agents_);
    };
  for(auto &p : ports)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Listener on ", p);
    auto task = std::make_shared<OefListenerStarterTask>(p, listeners, core, initialFactoryCreator);
    task -> submit();
  }
}
