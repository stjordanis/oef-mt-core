

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

bool quit = false;

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
    //std::cerr << "CLOSING..."<<std::endl;
    Eps local_eps;
    local_eps.swap(eps);
    for(auto& kv : local_eps)
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

      sleep(1);
    }
    //std::cerr << "NUKING..."<<std::endl;
  }

   void Error(int ident, const boost::system::error_code& ec)
  {
    //std::cout << "Error..." << std::endl;
    Lock lock(mutex);
    eps.erase(ident);
    std::cout << "Error: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  void Eof(int ident)
  {
    //std::cout << "Eof..." << std::endl;
    Lock lock(mutex);
    eps.erase(ident);
    std::cout << "Eof: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  void Start(int ident, EpP obj)
  {
    //std::cout << "Join..." << std::endl;
    Lock lock(mutex);
    eps[ident] = obj;
    std::cout << "Joined: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  std::shared_ptr<ISocketOwner> createNewConnection(Core &core)
  {
    Lock lock(mutex);
    counter+=1;

    auto ident = counter;

    auto endpoint = std::make_shared<Endpoint>(core, 2000, 2000);

    auto textLineMessageSenderPtr = std::make_shared<TextLineMessageSender>();
    auto textLineMessageReaderPtr = std::make_shared<TextLineMessageReader>();

    textLineMessageReaderPtr -> onComplete =
      [textLineMessageSenderPtr, endpoint](const std::string &s) {
      if (s=="shutdown")
      {
        quit = true;
      }
      textLineMessageSenderPtr -> send(s);
      endpoint -> run_sending();
    };

    endpoint -> reader = textLineMessageReaderPtr;
    endpoint -> writer = textLineMessageSenderPtr;
    endpoint -> onError = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
    endpoint -> onEof   = [this, ident](){ this->Eof(ident); };
    endpoint -> onStart = [this, ident, endpoint](){
      //std::cerr << "onStart handler" << std::endl;
      this->Start(ident, endpoint);
    }; // Capture sp by reference to avoid an increment until the call.

    return endpoint;
  }
private:
  Mutex mutex;
  Eps eps;
  int counter = 0;
};



int main(int argc, char *argv[])
{
  std::cerr << "ALLO" << std::endl;
  {
    Core core;

  EndpointCollection ec;

  Listener listener(core, 7600);
  listener.creator = [&ec](Core &core){ return ec.createNewConnection(core); };

  listener.start_accept();

  core.runThreaded(5);

  while(!quit)
  {
    sleep(1);
    std::cerr<< "tick"<<std::endl;
  }
  }
  std::cerr << "BYE" << std::endl;
}
