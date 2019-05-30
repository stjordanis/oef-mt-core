#pragma once

#include <vector>
#include <string>

class MtCoreArgs
{
public:

  MtCoreArgs()=default;
  virtual ~MtCoreArgs()=default;

  std::vector<int> listen_ports;
  std::size_t comms_thread_count = 10;
  std::size_t tasks_thread_count = 10;

  std::string search_uri;

protected:
private:
  MtCoreArgs(const MtCoreArgs &other) = delete;
  MtCoreArgs &operator=(const MtCoreArgs &other) = delete;
  bool operator==(const MtCoreArgs &other) = delete;
  bool operator<(const MtCoreArgs &other) = delete;
};
