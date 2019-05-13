#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <map>
#include <boost/asio.hpp>
#include <iostream>
#include <typeinfo>

class ISocketOwner;
class Core;

#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"

template<class TYPE>
class EndpointCollection : public std::enable_shared_from_this<EndpointCollection<TYPE>>
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using EpP = std::shared_ptr<TYPE>;
  using Eps = std::map<int, EpP>;

  std::function<void (void)> onKill;

  void visit(std::function<void (EpP endpoint)> visitor)
  {
    for(auto& kv : eps)
    {
      visitor(kv.second);
    }
  }

  EndpointCollection()
  {
  }

  void kill()
  {
    if (onKill)
    {
      onKill();
    }
  }

  ~EndpointCollection()
  {
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
}

  std::shared_ptr<ISocketOwner> createNewConnection(Core &core)
  {
    Lock lock(mutex);
    counter+=1;

    auto ident = counter;

    std::shared_ptr<TYPE> endpoint = std::make_shared<TYPE>(std::enable_shared_from_this<EndpointCollection<TYPE>>::shared_from_this(), ident, core);
    //std::cout << "New object:" << typeid(*endpoint).name() << std::endl;

    endpoint -> onError = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
    endpoint -> onEof   = [this, ident](){ this->Eof(ident); };
    endpoint -> onStart = [this, ident, endpoint](){
      this->Start(ident, endpoint);
    };
    return endpoint;
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
    std::cout << "Left: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

  void Start(int ident, EpP obj)
  {
    //std::cout << "Join..." << std::endl;
    Lock lock(mutex);
    eps[ident] = obj;
    std::cout << "Joined: " << ident << ". Current count = " << eps.size() << "." << std::endl;
  }

private:
  Mutex mutex;
  Eps eps;
  int counter = 0;
};
