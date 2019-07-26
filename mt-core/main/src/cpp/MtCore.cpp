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
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"

#include "mt-core/karma/src/cpp/KarmaPolicyBasic.hpp"
#include "mt-core/karma/src/cpp/KarmaPolicyNone.hpp"
#include "mt-core/karma/src/cpp/KarmaRefreshTask.hpp"

#include "google/protobuf/util/json_util.h"
#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/EndpointWebSocket.hpp"
#include <stdio.h>

#include <ctype.h>

#include "mt-core/secure/experimental/cpp/EndpointSSL.hpp"
#include "mt-core/oef-functions/src/cpp/InitialSslHandshakeTaskFactory.hpp"
#include "mt-core/oef-functions/src/cpp/InitialSecureHandshakeTaskFactory.hpp"
#include "mt-core/tasks/src/cpp/OefLoginTimeoutTask.hpp"

using namespace std::placeholders;

static const unsigned int minimum_thread_count = 1;

std::string prometheusUpThatNamingString(const std::string &name)
{
  std::string r;
  bool upshift = false;
  for(int i=0;i<name.length();i++)
  {
    auto c = name[i];

    switch(c)
    {
    case '-':
    case '_':
      upshift = true;
      break;
    case '.':
      r += "_";
      break;

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':

    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':

    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':

    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':

    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':

    case 'z':
      if (upshift)
      {
        r += std::string(1, ::toupper(c));
        upshift=false;
        break;
      }
    default:
      r += c;
      break;
    }
  }
  return r;
}

int MtCore::run()
{
  FETCH_LOG_INFO(LOGGING_NAME, "Starting core...");
  FETCH_LOG_INFO(LOGGING_NAME, "Core key: ", config_.core_key());
  FETCH_LOG_INFO(LOGGING_NAME, "Core URI: ", config_.core_uri());
  FETCH_LOG_INFO(LOGGING_NAME, "WebSocket URI: ", config_.ws_uri());
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

  comms_runners.start(std::max(config_.comms_thread_count(), minimum_thread_count), run_comms);
  tasks_runners.start(std::max(config_.tasks_thread_count(), minimum_thread_count), run_tasks);

  Uri core_uri(config_.core_uri());
  Uri search_uri(config_.search_uri());
  outbounds -> addConversationCreator("search", std::make_shared<OutboundSearchConversationCreator>(config_.core_key(),
      core_uri, search_uri, *core, outbounds));
  agents_ = std::make_shared<Agents>();

  if (config_.karma_policy().size())
  {
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = BASIC");
    karma_policy = std::make_shared<KarmaPolicyBasic>(config_.karma_policy());
    std::shared_ptr<Task> refresher = std::make_shared<KarmaRefreshTask>(karma_policy.get(), config_.karma_refresh_interval_ms());
    refresher -> submit();
  }
  else
  {
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = NONE");
    karma_policy = std::make_shared<KarmaPolicyNone>();
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA = NONE!!");
  }

  startListeners(karma_policy.get());

  Monitoring mon;
  auto mon_task = std::make_shared<MonitoringTask>();
  mon_task -> submit();

  std::map<std::string, std::string> prometheus_names;

  while(1)
  {
    tasks -> updateStatus();

    unsigned int snooze = 3;

    if (config_.prometheus_log_file().length())
    {
      if (config_.prometheus_log_interval())
      {
        snooze = config_.prometheus_log_interval();
      }

      std::string final_file = config_.prometheus_log_file();
      std::string temp_file = final_file + ".tmp";

      std::fstream fs;
      fs.open(temp_file.c_str(), std::fstream::out);
      if (fs.is_open())
      {
        mon.report([&fs, &prometheus_names](const std::string &name, std::size_t value){
            std::string new_name;
            auto new_name_iter = prometheus_names.find(name);
            if (new_name_iter == prometheus_names.end())
            {
              new_name = prometheusUpThatNamingString(name);
              prometheus_names[name] = new_name;
            }
            else
            {
              new_name = new_name_iter -> second;
            }

            if (new_name.find("_gauge_") != std::string::npos)
            {
              fs << "# TYPE " << new_name << " gauge" << std::endl;
            }
            else
            {
              new_name += "_total";
              fs << "# TYPE " << new_name << " counter" << std::endl;
            }
            fs << new_name << " " << value<< std::endl;
          });

        if (::rename(temp_file.c_str(), final_file.c_str()) != 0)
        {
          FETCH_LOG_WARN(LOGGING_NAME, "Could not create ", final_file);
        }
      }
      else
      {
        FETCH_LOG_WARN(LOGGING_NAME, "Could not create ", temp_file);
      }
    }
    else
    {
      FETCH_LOG_INFO(LOGGING_NAME, "----------------------------------------------");
      mon.report([](const std::string &name, std::size_t value){
          FETCH_LOG_INFO(LOGGING_NAME, name, ":", value);
        });
    }
    sleep(snooze);
  }
  return 0;
}

void MtCore::startListeners(IKarmaPolicy *karmaPolicy)
{
  std::size_t login_timeout_ms = 1000;
  std::chrono::milliseconds login_timeout(login_timeout_ms);

  IOefListener::FactoryCreator initialFactoryCreator =
    [this, login_timeout](std::shared_ptr<OefAgentEndpoint> endpoint)
    {
      endpoint -> addGoFunction([login_timeout](std::shared_ptr<OefAgentEndpoint> self){
          auto timeout = std::make_shared<OefLoginTimeoutTask>(self);
          timeout -> submit(login_timeout);
        });
      return std::make_shared<InitialHandshakeTaskFactory>(config_.core_key(), endpoint, outbounds, agents_);
    };

  Uri core_uri(config_.core_uri());
  FETCH_LOG_INFO(LOGGING_NAME, "Listener on ", core_uri.port);
  auto task = std::make_shared<OefListenerStarterTask<Endpoint>>(core_uri.port, listeners, core, initialFactoryCreator, karmaPolicy);
  task -> submit();
  if (!config_.ws_uri().empty())
  {
    Uri ws_uri(config_.ws_uri());
    FETCH_LOG_INFO(LOGGING_NAME, "Listener on ", ws_uri.port);
    auto task_ws = std::make_shared<OefListenerStarterTask<EndpointWebSocket>>(ws_uri.port, listeners, core, initialFactoryCreator, karmaPolicy);
    task_ws -> submit();
  }
  if (!config_.ssl_uri().empty())
  {
    initialFactoryCreator =
      [this, login_timeout](std::shared_ptr<OefAgentEndpoint> endpoint)
      {
        endpoint -> addGoFunction([login_timeout](std::shared_ptr<OefAgentEndpoint> self){
            auto timeout = std::make_shared<OefLoginTimeoutTask>(self);
            timeout -> submit(login_timeout);
          });
        return std::make_shared<InitialSslHandshakeTaskFactory>(config_.core_key(), endpoint, outbounds, agents_);
      };

    Uri ssl_uri(config_.ssl_uri());
    FETCH_LOG_INFO(LOGGING_NAME, "TLS/SSL Listener on ", ssl_uri.port);
    auto task_ssl = std::make_shared<OefListenerStarterTask<EndpointSSL>>(ssl_uri.port, listeners, core, initialFactoryCreator, karmaPolicy);
    task_ssl -> submit();
  }
  if (!config_.secure_uri().empty())
  {
    initialFactoryCreator =
      [this, login_timeout](std::shared_ptr<OefAgentEndpoint> endpoint)
      {
         endpoint -> addGoFunction([login_timeout](std::shared_ptr<OefAgentEndpoint> self){
            auto timeout = std::make_shared<OefLoginTimeoutTask>(self);
            timeout -> submit(login_timeout);
          });
         return std::make_shared<InitialSecureHandshakeTaskFactory>(config_.core_key(), endpoint, outbounds, agents_);
      };

    Uri secure_uri(config_.secure_uri());
    FETCH_LOG_INFO(LOGGING_NAME, "Secure Listener on ", secure_uri.port);
    auto task_secure = std::make_shared<OefListenerStarterTask<Endpoint>>(secure_uri.port, listeners, core, initialFactoryCreator, karmaPolicy);
    task_secure -> submit();
  }
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
  return false;
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
  google::protobuf::util::JsonParseOptions options;
  options.ignore_unknown_fields = true;

  auto status = google::protobuf::util::JsonStringToMessage(config_json, &config_, options);
  if (!status.ok()) {
    FETCH_LOG_ERROR(LOGGING_NAME, "Parse error: '" + status.ToString() + "'");
  }
  return status.ok();
}
