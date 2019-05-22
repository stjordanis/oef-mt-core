#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <map>
#include <vector>
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
  using EpIdent = int;
  using Eps = std::map<EpIdent, EpP>;
  using EpIdents = std::vector<EpIdent>;

  std::function<void (void)> onKill;

  void visit(std::function<void (EpP endpoint)> visitor)
  {
    std::vector<int> endpoint_idents = getEndpointIdents();

    for(auto& endpoint_ident : endpoint_idents)
    {
      EpP p;
      {
        Lock lock(mutex);
        auto kvi = eps.find(endpoint_ident);
        if (kvi != eps.end())
        {
          p = kvi -> second;
        }
      }
      visitor(p);
    }
  }

  EpIdents getEndpointIdents()
  {
    std::vector<int> endpoint_idents;
    endpoint_idents.reserve(eps.size());
    {
      Lock lock(mutex);
      for(auto& kv : eps)
      {
        endpoint_idents.push_back(kv.first);
      }
    }
    return endpoint_idents;
  }

  EndpointCollection()
  {
  }

  EpP getEndpoint(EpIdent ident)
  {
    Lock lock(mutex);
    auto kv = eps.find(ident);
    if (kv != eps.end())
    {
      return kv -> second;
    }
    return EpP();
  }

  void kill()
  {
    if (onKill)
    {
      onKill();
    }
  }

  void close(EpIdent ident)
  {
    //std::cout << "close?: " << ident << std::endl;
    Lock lock(mutex);
    auto kv = eps.find(ident);
    if (kv != eps.end())
    {
      //std::cout << "close: " << ident << std::endl;
      kv -> second -> close();
      eps.erase(ident);
    }
    else
    {
      //std::cout << "notfound: " << ident << std::endl;
    }
  }

  ~EndpointCollection()
  {
    {
      Lock lock(mutex);
      Eps local_eps;
      local_eps.swap(eps);
      for(auto& kv : local_eps)
      {
        kv.second -> close();
      }
    }

    while(true)
    {
      int count = 0;
      {
        count = eps.size();
      }

    if (count == 0)
    {
      break;
    }

    sleep(1);
  }
}

  std::shared_ptr<TYPE> createNewConnection(Core &core)
  {
    Lock lock(mutex);
    counter+=1;

    auto ident = counter;

    std::shared_ptr<TYPE> endpoint = std::make_shared<TYPE>(std::enable_shared_from_this<EndpointCollection<TYPE>>::shared_from_this(), ident, core);

    endpoint -> onError        = [this, ident](const boost::system::error_code& ec){ this->Error(ident, ec); };
    endpoint -> onEof          = [this, ident](){ this->Eof(ident); };
    endpoint -> onProtoError   = [this, ident](const std::string &msg){ this->ProtoError(ident, msg); };
    endpoint -> onStart        = [this, ident, endpoint](){ this->Start(ident, endpoint); };
    return endpoint;
  }

  void Error(int ident, const boost::system::error_code& ec)
  {
    close(ident);
    //std::cout << "Error: " << ident << ". Current count = " << size() << "." << std::endl;
  }

  void ProtoError(int ident, const std::string &msg)
  {
    close(ident);
    //std::cout << "ProtoError: " << msg << std::endl;
  }

  void Eof(int ident)
  {
    close(ident);
    //std::cout << "Left: " << ident << ". Current count = " << size() << "." << std::endl;
  }

  std::size_t size(void)
  {
    Lock lock(mutex);
    return eps.size();
  }

  void Start(int ident, EpP obj)
  {
    {
      Lock lock(mutex);
      eps[ident] = obj;
    }
    //std::cout << "Joined: " << ident << ". Current count = " << size() << "." << std::endl;
  }

private:
  Mutex mutex;
  Eps eps;
  int counter = 0;
};
