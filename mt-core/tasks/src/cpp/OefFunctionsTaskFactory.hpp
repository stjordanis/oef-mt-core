#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class OefFunctionsTaskFactory
{
public:
  OefFunctionsTaskFactory()
  {
  }
  virtual ~OefFunctionsTaskFactory()
  {
  }

  //friend std::ostream& operator<<(std::ostream& os, const OefFunctionsTaskFactory &output);
  //friend void swap(OefFunctionsTaskFactory &a, OefFunctionsTaskFactory &b);
protected:
  // int compare(const OefFunctionsTaskFactory &other) const { ... }
  // void copy(const OefFunctionsTaskFactory &other) { ... }
  // void clear(void) { ... }
  // bool empty(void) const { ... }
  // void swap(OefFunctionsTaskFactory &other) { ... }
private:
  OefFunctionsTaskFactory(const OefFunctionsTaskFactory &other) = delete; // { copy(other); }
  OefFunctionsTaskFactory &operator=(const OefFunctionsTaskFactory &other) = delete; // { copy(other); return *this; }
  bool operator==(const OefFunctionsTaskFactory &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OefFunctionsTaskFactory &other) = delete; // const { return compare(other)==-1; }

  //bool operator!=(const OefFunctionsTaskFactory &other) const { return compare(other)!=0; }
  //bool operator>(const OefFunctionsTaskFactory &other) const { return compare(other)==1; }
  //bool operator<=(const OefFunctionsTaskFactory &other) const { return compare(other)!=1; }
  //bool operator>=(const OefFunctionsTaskFactory &other) const { return compare(other)!=-1; }
};

//namespace std { template<> void swap(OefFunctionsTaskFactory& lhs, OefFunctionsTaskFactory& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const OefFunctionsTaskFactory &output) {}
