#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class IOefAgentTaskFactory
{
public:
  IOefAgentTaskFactory()
  {
  }
  virtual ~IOefAgentTaskFactory()
  {
  }

  //friend std::ostream& operator<<(std::ostream& os, const IOefAgentTaskFactory &output);
  //friend void swap(IOefAgentTaskFactory &a, IOefAgentTaskFactory &b);
protected:
  // int compare(const IOefAgentTaskFactory &other) const { ... }
  // void copy(const IOefAgentTaskFactory &other) { ... }
  // void clear(void) { ... }
  // bool empty(void) const { ... }
  // void swap(IOefAgentTaskFactory &other) { ... }
private:
  IOefAgentTaskFactory(const IOefAgentTaskFactory &other) = delete; // { copy(other); }
  IOefAgentTaskFactory &operator=(const IOefAgentTaskFactory &other) = delete; // { copy(other); return *this; }
  bool operator==(const IOefAgentTaskFactory &other) = delete; // const { return compare(other)==0; }
  bool operator<(const IOefAgentTaskFactory &other) = delete; // const { return compare(other)==-1; }

  //bool operator!=(const IOefAgentTaskFactory &other) const { return compare(other)!=0; }
  //bool operator>(const IOefAgentTaskFactory &other) const { return compare(other)==1; }
  //bool operator<=(const IOefAgentTaskFactory &other) const { return compare(other)!=1; }
  //bool operator>=(const IOefAgentTaskFactory &other) const { return compare(other)!=-1; }
};

//namespace std { template<> void swap(IOefAgentTaskFactory& lhs, IOefAgentTaskFactory& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const IOefAgentTaskFactory &output) {}
