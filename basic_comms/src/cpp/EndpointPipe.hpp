#pragma once

#include "threading/src/cpp/lib/Notification.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <memory>


template <typename EndpointType, typename SendType>
class EndpointPipe
    : public ISocketOwner
{
public:

  explicit EndpointPipe(std::shared_ptr<EndpointType> endpoint)
      : endpoint(std::move(endpoint))
  {
  }

  virtual ~EndpointPipe()
  {
  }


  virtual Notification::NotificationBuilder send(std::shared_ptr<SendType> s)
  {
    return endpoint->send(s);
  }

  virtual void go() override
  {
    endpoint->go();
  }

  virtual ISocketOwner::Socket& socket() override
  {
    return endpoint->socket();
  }

  virtual void run_sending()
  {
    endpoint->run_sending();
  }

  virtual bool IsTXQFull()
  {
    return endpoint->IsTXQFull();
  }

  virtual void wake()
  {
    endpoint->wake();
  }
  
  std::size_t getIdent(void) const { return endpoint->getIdent(); }

  virtual const std::string &getRemoteId(void) const
  {
    return endpoint -> getRemoteId();
  }

protected:
  std::shared_ptr<EndpointType> endpoint;

private:
  EndpointPipe(const EndpointPipe &other) = delete;
  EndpointPipe &operator=(const EndpointPipe &other) = delete;
  bool operator==(const EndpointPipe &other) = delete;
  bool operator<(const EndpointPipe &other) = delete;
};
