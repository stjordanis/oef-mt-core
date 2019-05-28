#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class OutboundPool
{
public:
  OutboundPool()
  {
  }
  virtual ~OutboundPool()
  {
  }

  void housekeeping();

  template<class ENDPOINT_CLASS>
  std::shared_ptr<ENDPOINT_CLASS>

protected:
private:
  OutboundPool(const OutboundPool &other) = delete; // { copy(other); }
  OutboundPool &operator=(const OutboundPool &other) = delete; // { copy(other); return *this; }
  bool operator==(const OutboundPool &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OutboundPool &other) = delete; // const { return compare(other)==-1; }
};
