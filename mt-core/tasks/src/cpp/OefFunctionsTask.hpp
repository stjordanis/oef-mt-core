#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class OefFunctionsTask
{
public:
  OefFunctionsTask()
  {
  }
  virtual ~OefFunctionsTask()
  {
  }

  //friend std::ostream& operator<<(std::ostream& os, const OefFunctionsTask &output);
  //friend void swap(OefFunctionsTask &a, OefFunctionsTask &b);
protected:
  // int compare(const OefFunctionsTask &other) const { ... }
  // void copy(const OefFunctionsTask &other) { ... }
  // void clear(void) { ... }
  // bool empty(void) const { ... }
  // void swap(OefFunctionsTask &other) { ... }
private:
  OefFunctionsTask(const OefFunctionsTask &other) = delete; // { copy(other); }
  OefFunctionsTask &operator=(const OefFunctionsTask &other) = delete; // { copy(other); return *this; }
  bool operator==(const OefFunctionsTask &other) = delete; // const { return compare(other)==0; }
  bool operator<(const OefFunctionsTask &other) = delete; // const { return compare(other)==-1; }

  //bool operator!=(const OefFunctionsTask &other) const { return compare(other)!=0; }
  //bool operator>(const OefFunctionsTask &other) const { return compare(other)==1; }
  //bool operator<=(const OefFunctionsTask &other) const { return compare(other)!=1; }
  //bool operator>=(const OefFunctionsTask &other) const { return compare(other)!=-1; }
};

//namespace std { template<> void swap(OefFunctionsTask& lhs, OefFunctionsTask& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const OefFunctionsTask &output) {}
