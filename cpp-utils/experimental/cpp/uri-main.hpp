#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class uri-main
{
public:
  uri-main()
  {
  }
  virtual ~uri-main()
  {
  }

  //friend std::ostream& operator<<(std::ostream& os, const uri-main &output);
  //friend void swap(uri-main &a, uri-main &b);
protected:
  // int compare(const uri-main &other) const { ... }
  // void copy(const uri-main &other) { ... }
  // void clear(void) { ... }
  // bool empty(void) const { ... }
  // void swap(uri-main &other) { ... }
private:
  uri-main(const uri-main &other) = delete; // { copy(other); }
  uri-main &operator=(const uri-main &other) = delete; // { copy(other); return *this; }
  bool operator==(const uri-main &other) = delete; // const { return compare(other)==0; }
  bool operator<(const uri-main &other) = delete; // const { return compare(other)==-1; }

  //bool operator!=(const uri-main &other) const { return compare(other)!=0; }
  //bool operator>(const uri-main &other) const { return compare(other)==1; }
  //bool operator<=(const uri-main &other) const { return compare(other)!=1; }
  //bool operator>=(const uri-main &other) const { return compare(other)!=-1; }
};

//namespace std { template<> void swap(uri-main& lhs, uri-main& rhs) { lhs.swap(rhs); } }
//std::ostream& operator<<(std::ostream& os, const uri-main &output) {}
