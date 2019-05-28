#pragma once

#include <memory>

#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"

class Core;

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class OefEndpoint: public ProtoMessageEndpoint, public std::enable_shared_from_this<OefEndpoint>
{
public:
  OefEndpoint(Core &core):ProtoMessageEndpoint(core)
  {
  }
  virtual ~OefEndpoint()
  {
  }

  void setup(std::shared_ptr<OefEndpoint> myself)
  {
    ProtoMessageEndpoint::setup(myself);
  }
protected:
private:
  OefEndpoint(const OefEndpoint &other) = delete;
  OefEndpoint &operator=(const OefEndpoint &other) = delete;
  bool operator==(const OefEndpoint &other) = delete;
  bool operator<(const OefEndpoint &other) = delete;
};
