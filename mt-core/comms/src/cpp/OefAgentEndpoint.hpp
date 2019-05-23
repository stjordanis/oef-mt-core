#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class OefAgentEndpoint
{
public:
  OefAgentEndpoint()
  {
  }
  virtual ~OefAgentEndpoint()
  {
  }

  //friend std::ostream& operator<<(std::ostream& os, const OefAgentEndpoint &output);
  //friend void swap(OefAgentEndpoint &a, OefAgentEndpoint &b);
protected:
  // int compare(const OefAgentEndpoint &other) const { ... }
  // void copy(const OefAgentEndpoint &other) { ... }
  // void clear(void) { ... }
  // bool empty(void) const { ... }
  // void swap(OefAgentEndpoint &other) { ... }
private:
  OefAgentEndpoint(const OefAgentEndpoint &other) = delete; // { copy(other); }
  OefAgentEndpoint &operator=(const OefAgentEndpoint &other) = delete; // { copy(other); return *this; }
  bool operator==(const OefAgentEndpoint &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OefAgentEndpoint &other) = delete; // const { return compare(other)==-1; }

  //bool operator!=(const OefAgentEndpoint &other) const { return compare(other)!=0; }
  //bool operator>(const OefAgentEndpoint &other) const { return compare(other)==1; }
  //bool operator<=(const OefAgentEndpoint &other) const { return compare(other)!=1; }
  //bool operator>=(const OefAgentEndpoint &other) const { return compare(other)!=-1; }
};

//namespace std { template<> void swap(OefAgentEndpoint& lhs, OefAgentEndpoint& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const OefAgentEndpoint &output) {}
