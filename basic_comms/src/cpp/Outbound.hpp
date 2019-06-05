#pragma once

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

class Outbound:public Endpoint
{
public:
  Outbound(const Uri &uri
           , Core &core
           ,std::size_t sendBufferSize
           ,std::size_t readBufferSize)
    : Endpoint(core, sendBufferSize, readBufferSize)
    , uri(uri)
    , core(core)
  {
    this -> uri = uri;
  }
  virtual ~Outbound()
  {
  }

  // run this in a thread.
  bool run(void);
protected:
  Uri uri;
  Core &core;
private:
  Outbound(const Outbound &other) = delete; // { copy(other); }
  Outbound &operator=(const Outbound &other) = delete; // { copy(other); return *this; }
  bool operator==(const Outbound &other) = delete; // const { return compare(other)==0; }
  bool operator<(const Outbound &other) = delete; // const { return compare(other)==-1; }


};

//namespace std { template<> void swap(Outbound& lhs, Outbound& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const Outbound &output) {}
