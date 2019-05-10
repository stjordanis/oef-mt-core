#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <map>
#include <boost/asio.hpp>
#include <iostream>

class ChatEndpoint;
class ISocketOwner;
class Core;

class EndpointCollection: public std::enable_shared_from_this<EndpointCollection>
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using EpP = std::shared_ptr<ChatEndpoint>;
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

  ~EndpointCollection();

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

  void kill(void);

  std::shared_ptr<ISocketOwner> createNewConnection(Core &core);
private:
  Mutex mutex;
  Eps eps;
  int counter = 0;
};
