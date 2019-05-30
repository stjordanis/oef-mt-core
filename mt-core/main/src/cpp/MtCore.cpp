#include "MtCore.hpp"

#include <iostream>
#
#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/tasks/src/cpp/InitialHandshakeTaskFactory.hpp"
#include "mt-core/conversations/src/cpp/OutboundSearchConversationCreator.hpp"

using namespace std::placeholders;

int MtCore::run(const MtCore::args &args)
{
  listeners = std::make_shared<OefListenerSet>();
  core = std::make_shared<Core>();
  auto tasks = std::make_shared<Taskpool>();
  tasks -> setDefault();

  std::function<void (void)> run_comms = std::bind(&Core::run, core.get());
  std::function<void (std::size_t thread_number)> run_tasks = std::bind(&Taskpool::run, tasks.get(), _1);

  std::cout << "comms_thread_count " << args.comms_thread_count <<std::endl;
  std::cout << "tasks_thread_count " << args.tasks_thread_count <<std::endl;

  comms_runners.start(args.comms_thread_count, run_comms);
  tasks_runners.start(args.tasks_thread_count, run_tasks);

  //outbounds -> addConversationCreator("search", std::make_shared<OutboundSearchConversationCreator>(args.search_uri, *core));

  startListeners(args.listen_ports);

  while(1)
  {
    std::cout << "ok" << std::endl;
    sleep(10);
  }
  return 0;
}

void MtCore::startListeners(const std::vector<int> &ports)
{
  IOefListener::FactoryCreator initialFactoryCreator =
    [](std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      return std::make_shared<InitialHandshakeTaskFactory>(endpoint);
    };
  for(auto &p : ports)
  {
    std::cout << "Listener on "<< p << std::endl;
    auto task = std::make_shared<OefListenerStarterTask>(p, listeners, core, initialFactoryCreator);
    task -> submit();
  }
}
