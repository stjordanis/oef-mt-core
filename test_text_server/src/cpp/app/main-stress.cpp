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

std::mutex mutex;
std::condition_variable quit;

using namespace std::placeholders;

#include "test_text_server/src/cpp/lib/ProtoChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/BroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/StressInducingBroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

using ProtoChatEndpointCollection = EndpointCollection<ProtoChatEndpoint>;

int main(int argc, char *argv[])
{
  std::cerr << "ALLO" << std::endl;
  {
    Core core;

    auto tasks = std::make_shared<Taskpool>();
    tasks -> setDefault();

    auto ec = std::make_shared<ProtoChatEndpointCollection>();

    ec -> onKill = [](){
      std::unique_lock<std::mutex> lock(mutex);
      quit.notify_all();
    };

    Listener listener(core, 7600);
    listener.creator = [ec](Core &core){
      auto ep = ec -> createNewConnection(core);
      auto id = ep -> id;

      ep -> protoTextLineMessageReader -> onComplete =
      [ec, id](ProtoChatEndpoint::Data s) {
        if (s -> contents() == "shutdown")
        {
          ec -> kill();
        }
        else
        {
          auto task = std::make_shared<StressInducingBroadcastTask>(ec, s, id);
          task -> submit();
        }
      };
      return ep;
    };

    listener.start_accept();

    Threadpool core_runners;
    Threadpool task_runners;

    std::function<void (void)> run_core = std::bind(&Core::run, &core);
    std::function<void (std::size_t thread_number)> run_task = std::bind(&Taskpool::run, tasks.get(), _1);

    core_runners.start(5, run_core);
    task_runners.start(5, run_task);

    {
      std::unique_lock<std::mutex> lock(mutex);
      quit.wait(lock);
    }

    std::cerr<< "QUIT"<<std::endl;
    tasks -> stop();
    core.stop();
    core_runners.stop();
    task_runners.stop();
  }
  std::cerr << "BYE" << std::endl;
}
