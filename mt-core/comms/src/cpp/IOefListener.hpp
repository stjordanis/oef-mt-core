#pragma once

// Delete bits as needed

//#include <algorithm>
//#include <utility>
//#include <iostream>

class IOefListener
{
public:
  IOefListener()
  {
  }
  virtual ~IOefListener()
  {
  }

protected:
private:
  IOefListener(const IOefListener &other) = delete;
  IOefListener &operator=(const IOefListener &other) = delete;
  bool operator==(const IOefListener &other) = delete;
  bool operator<(const IOefListener &other) = delete;
};

