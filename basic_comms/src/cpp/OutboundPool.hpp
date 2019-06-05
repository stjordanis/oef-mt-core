#pragma once

#include <memory>
#include <string>

#include "mt-core/comms/src/cpp/ProtoMessageEndpoint.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

class OutboundPool
{
public:
  using Endpoint = ProtoMessageEndpoint;
  using EndpointP = std::shared_ptr<Endpoint>;
  using EndpointName = std::string;
  using EndpointNameToUri = std::map<EndpointName, Uri>;
  using EndpointNameToPtr = std::map<EndpointName, EndpointP>;

  OutboundPool()
  {
  }
  virtual ~OutboundPool()
  {
  }

  void add(const EndpointName &name, consr Uri &uri);

  EndpointP get(

  void housekeeping();

protected:
  EndpointNameToUri nameToUri;
  EndpointNameToPtr nameToPrt;
private:
  OutboundPool(const OutboundPool &other) = delete; // { copy(other); }
  OutboundPool &operator=(const OutboundPool &other) = delete; // { copy(other); return *this; }
  bool operator==(const OutboundPool &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OutboundPool &other) = delete; // const { return compare(other)==-1; }
};
