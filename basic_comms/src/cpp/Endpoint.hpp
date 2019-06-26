#pragma once

#include "basic_comms/src/cpp/EndpointBase.hpp"
#include "basic_comms/src/cpp/RingBuffer.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include <boost/asio.hpp>

#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include <iostream>


template <typename TXType>
class Endpoint
  : public EndpointBase<TXType>
  , public std::enable_shared_from_this<Endpoint<TXType>>
{
public:
  using EndpointBase<TXType>::state;
  using EndpointBase<TXType>::readBuffer;
  using EndpointBase<TXType>::sendBuffer;

  using Socket = typename EndpointBase<TXType>::Socket;

  static constexpr char const *LOGGING_NAME = "Endpoint";

  Endpoint(const Endpoint &other) = delete;
  Endpoint &operator=(const Endpoint &other) = delete;
  bool operator==(const Endpoint &other) = delete;
  bool operator<(const Endpoint &other) = delete;

  Endpoint(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
  );

  virtual ~Endpoint();

  virtual Socket& socket()
  {
    return sock;
  }

protected:
  virtual void async_read(const std::size_t& bytes_needed);
  virtual void async_write();

protected:
  Socket sock;
};
