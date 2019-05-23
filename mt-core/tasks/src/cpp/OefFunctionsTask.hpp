#pragma once

class OefFunctionsTask: public IMtCoreTask
{
public:
  OefFunctionsTask()
  {
  }
  virtual ~OefFunctionsTask()
  {
  }
protected:
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
