#include "MtCore.hpp"

#include <iostream>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/oef-functions/src/cpp/InitialHandshakeTaskFactory.hpp"
#include "mt-core/conversations/src/cpp/OutboundSearchConversationCreator.hpp"
#include "monitoring/src/cpp/lib/Monitoring.hpp"

using namespace std::placeholders;

int MtCore::run(const MtCore::args &args)
{
  FETCH_LOG_INFO(LOGGING_NAME, "Starting core...");
  FETCH_LOG_INFO(LOGGING_NAME, "Core key: ", args.core_key);
  FETCH_LOG_INFO(LOGGING_NAME, "Core URI: ", args.core_uri_str);
  FETCH_LOG_INFO(LOGGING_NAME, "Search URI: ", args.search_uri_str);

  core_key_ = args.core_key;

  listeners = std::make_shared<OefListenerSet>();
  core = std::make_shared<Core>();
  auto tasks = std::make_shared<Taskpool>();
  tasks -> setDefault();
  outbounds = std::make_shared<OutboundConversations>();

  std::function<void (void)> run_comms = std::bind(&Core::run, core.get());
  std::function<void (std::size_t thread_number)> run_tasks = std::bind(&Taskpool::run, tasks.get(), _1);

  FETCH_LOG_INFO(LOGGING_NAME, "comms_thread_count ", args.comms_thread_count);
  FETCH_LOG_INFO(LOGGING_NAME, "tasks_thread_count ", args.tasks_thread_count);

  comms_runners.start(args.comms_thread_count, run_comms);
  tasks_runners.start(args.tasks_thread_count, run_tasks);

  outbounds -> addConversationCreator("search", std::make_shared<OutboundSearchConversationCreator>(args.core_key,
      args.core_uri, args.search_uri, *core, outbounds));
  agents_ = std::make_shared<Agents>();

  startListeners(args.listen_ports);

  Monitoring mon;

  while(1)
  {
    //auto s = tasks -> getStatus();

    mon.report([](const std::string &name, std::size_t value){
        FETCH_LOG_INFO(LOGGING_NAME, name, ":", value);
      });
    sleep(3);
  }
  return 0;
}

void MtCore::startListeners(const std::vector<uint16_t> &ports)
{
  IOefListener::FactoryCreator initialFactoryCreator =
    [this](std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      return std::make_shared<InitialHandshakeTaskFactory>(core_key_, endpoint, outbounds, agents_);
    };
  for(auto &p : ports)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Listener on ", p);
    auto task = std::make_shared<OefListenerStarterTask>(p, listeners, core, initialFactoryCreator);
    task -> submit();
  }
}
