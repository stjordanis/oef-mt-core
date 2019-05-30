#pragma once

#include <memory>

#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "fetch_teams/ledger/logger.hpp"

class Core;
class Uri;

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
  static constexpr char const *LOGGING_NAME = "OefEndpoint";

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

  bool connect(const Uri &uri, Core &core);
protected:
private:
  OefEndpoint(const OefEndpoint &other) = delete;
  OefEndpoint &operator=(const OefEndpoint &other) = delete;
  bool operator==(const OefEndpoint &other) = delete;
  bool operator<(const OefEndpoint &other) = delete;
};
