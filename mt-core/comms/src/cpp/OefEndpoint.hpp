#pragma once

#include <memory>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/Endpoint.hpp"

class Core;

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class OefEndpoint: public Endpoint, public std::enable_shared_from_this<OefEndpoint>
{
public:
  OefEndpoint(Core &core):Endpoint(core, 10000, 10000)
  {
  }
  virtual ~OefEndpoint()
  {
  }
protected:
private:
  OefEndpoint(const OefEndpoint &other) = delete;
  OefEndpoint &operator=(const OefEndpoint &other) = delete;
  bool operator==(const OefEndpoint &other) = delete;
  bool operator<(const OefEndpoint &other) = delete;
};
