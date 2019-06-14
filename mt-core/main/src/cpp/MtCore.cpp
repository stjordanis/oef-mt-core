#include "MtCore.hpp"

#include <iostream>
#include <fstream>

#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"
#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "mt-core/oef-functions/src/cpp/InitialHandshakeTaskFactory.hpp"
#include "mt-core/conversations/src/cpp/OutboundSearchConversationCreator.hpp"
#include "monitoring/src/cpp/lib/Monitoring.hpp"
#include "mt-core/status/src/cpp/MonitoringTask.hpp"

#include "mt-core/karma/src/cpp/KarmaPolicyBasic.hpp"
#include "mt-core/karma/src/cpp/KarmaPolicyNone.hpp"

#include "google/protobuf/util/json_util.h"

using namespace std::placeholders;

int MtCore::run()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Starting core...");
  FETCH_LOG_INFO(LOGGING_NAME, "Core key: ", config_.core_key());
  FETCH_LOG_INFO(LOGGING_NAME, "Core URI: ", config_.core_uri());
  FETCH_LOG_INFO(LOGGING_NAME, "Search URI: ", config_.search_uri());
  FETCH_LOG_INFO(LOGGING_NAME, "comms_thread_count: ", config_.comms_thread_count());
  FETCH_LOG_INFO(LOGGING_NAME, "tasks_thread_count: ", config_.tasks_thread_count());

  listeners = std::make_shared<OefListenerSet>();
  core = std::make_shared<Core>();
  auto tasks = std::make_shared<Taskpool>();
  tasks -> setDefault();
  outbounds = std::make_shared<OutboundConversations>();

  std::function<void (void)> run_comms = std::bind(&Core::run, core.get());
  std::function<void (std::size_t thread_number)> run_tasks = std::bind(&Taskpool::run, tasks.get(), _1);

  comms_runners.start(config_.comms_thread_count(), run_comms);
  tasks_runners.start(config_.tasks_thread_count(), run_tasks);

  Uri core_uri(config_.core_uri());
  Uri search_uri(config_.search_uri());
  outbounds -> addConversationCreator("search", std::make_shared<OutboundSearchConversationCreator>(config_.core_key(),
      core_uri, search_uri, *core, outbounds));
  agents_ = std::make_shared<Agents>();

  if (config_.karma_policy().size())
  {
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = BASIC");
    karma_policy = std::make_shared<KarmaPolicyBasic>(config_.karma_policy());
  }
  else
  {
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = NONE");
    karma_policy = std::make_shared<KarmaPolicyNone>(77);
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = NONE!!");
  }

  startListeners();

  Monitoring mon;
  auto mon_task = std::make_shared<MonitoringTask>();
  mon_task -> submit();

  while(1)
  {
    //auto s = tasks -> getStatus();

    FETCH_LOG_INFO(LOGGING_NAME, "----------------------------------------------");
    mon.report([](const std::string &name, std::size_t value){
        FETCH_LOG_INFO(LOGGING_NAME, name, ":", value);
      });
    tasks -> getFinishedTasks();
    sleep(3);
  }
  return 0;
}

void MtCore::startListeners()
{
  IOefListener::FactoryCreator initialFactoryCreator =
    [this](std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      return std::make_shared<InitialHandshakeTaskFactory>(config_.core_key(), endpoint, outbounds, agents_);
    };

  Uri core_uri(config_.core_uri());
  FETCH_LOG_INFO(LOGGING_NAME, "Listener on ", core_uri.port);
  auto task = std::make_shared<OefListenerStarterTask>(core_uri.port, listeners, core, initialFactoryCreator);
  task -> submit();
}


bool MtCore::configure(const std::string &config_file, const std::string &config_json)
{
  if (config_file.length())
  {
    return configureFromJsonFile(config_file);
  }
  if (config_json.length())
  {
    return configureFromJson(config_json);
  }
}

bool MtCore::configureFromJsonFile(const std::string &config_file)
{
  std::ifstream json_file(config_file);

  std::string json = "";
  if (!json_file.is_open())
  {
    FETCH_LOG_ERROR(LOGGING_NAME, "Failed to load configuration: '" + config_file + "'");
    return false;
  }
  else
  {
    std::string line;
    while (std::getline(json_file, line))
    {
      json += line;
    }
  }
  return configureFromJson(json);
}


bool MtCore::configureFromJson(const std::string &config_json)
{

  auto status = google::protobuf::util::JsonStringToMessage(config_json, &config_);
  if (!status.ok()) {
    FETCH_LOG_ERROR(LOGGING_NAME, "Parse error: '" + status.ToString() + "'");
  }
  return status.ok();
}