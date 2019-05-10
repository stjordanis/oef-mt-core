#include <list>
#include <iostream>
#include <functional>
#include <memory>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/Listener.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"

#include "threading/src/cpp/lib/Threadpool.hpp"
#include "threading/src/cpp/lib/Taskpool.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

bool quit = false;

using namespace std::placeholders;

class EndpointCollection;
class ChatEndpoint;
class BroadcastTask;

#include "test_text_server/src/cpp/lib/ChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/BroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

int main(int argc, char *argv[])
{
  std::cerr << "ALLO" << std::endl;
  {
    Core core;

    auto tasks = std::make_shared<Taskpool>();
    tasks -> setDefault();

    auto ec = std::make_shared<EndpointCollection>();

    ec -> onKill = [](){ quit = true; };

    Listener listener(core, 7600);
    listener.creator = [ec](Core &core){ return ec -> createNewConnection(core); };

    listener.start_accept();

    Threadpool core_runners;
    Threadpool task_runners;

    std::function<void (void)> run_core = std::bind(&Core::run, &core);
    std::function<void (std::size_t thread_number)> run_task = std::bind(&Taskpool::run, tasks.get(), _1);

    core_runners.start(5, run_core);
    task_runners.start(5, run_task);

    while(!quit)
    {
      sleep(1);
      std::cerr<< "tick"<<std::endl;
    }
    std::cerr<< "quit"<<std::endl;

    tasks -> stop();
    std::cerr<< "quit"<<std::endl;
    core.stop();
    std::cerr<< "quit"<<std::endl;

    core_runners.stop();
    std::cerr<< "quit"<<std::endl;
    task_runners.stop();
    std::cerr<< "quit"<<std::endl;
  }
  std::cerr << "BYE" << std::endl;

}
