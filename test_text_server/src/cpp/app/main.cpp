

#include <list>
#include <iostream>
#include <functional>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/Listener.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

class EndpointCollection
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using EpP = std::shared_ptr<Endpoint>;
  using Eps = std::map<int, EpP>;

  EndpointCollection()
  {
  }

  ~EndpointCollection()
  {
    for(auto& kv : eps)
    {
      kv.second -> close();
    }
    while(true)
    {
      int count = 0;
      {
        Lock lock(mutex);
        count = eps.size();
      }
      if (count == 0)
      {
        break;
      }
      std::cerr << "Waiting for shutdowns "<< count << std::endl;
      sleep(1);
    }
  }

   void Error(int ident, const boost::system::error_code& ec)
  {
    std::cout << "Error..." << std::endl;
    eps.erase(ident);
    std::cout << "Error: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  void Eof(int ident)
  {
    std::cout << "Eof..." << std::endl;
    eps.erase(ident);
    std::cout << "Eof: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  void Start(int ident, EpP obj)
  {
    std::cout << "Join..." << std::endl;
    eps[ident] = obj;
    std::cout << "Joined: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  std::shared_ptr<ISocketOwner> createNewConnection(Core &core)
  {
    Lock lock(mutex);
    counter+=1;

    auto ident = counter;

    auto obj = std::make_shared<Endpoint>(core, 2000, 2000);

    auto textLineMessageSenderPtr = std::make_shared<TextLineMessageSender>();
    auto textLineMessageReaderPtr = std::make_shared<TextLineMessageReader>(textLineMessageSenderPtr, obj);

    obj -> reader = textLineMessageReaderPtr;
    obj -> writer = textLineMessageSenderPtr;
    obj -> onError = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
    obj -> onEof   = [this, ident](){ this->Eof(ident); };
    obj -> onStart = [this, ident, &obj](){ this->Start(ident, obj); }; // Capture sp by reference to avoid an increment until the call.

    return obj;
  }
private:
  Mutex mutex;
  Eps eps;
  int counter = 0;
};



int main(int argc, char *argv[])
{
  Core core;

  EndpointCollection ec;

  Listener listener(core);
  listener.creator = [&ec](Core &core){ return ec.createNewConnection(core); };

  listener.start_accept();

  core.run();
}
