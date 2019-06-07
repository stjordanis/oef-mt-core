#pragma once

#include <vector>
#include <string>

#include "cpp-utils/src/cpp/lib/Uri.hpp"

class MtCoreArgs
{
public:

  MtCoreArgs()= default;
  virtual ~MtCoreArgs()=default;

  void init()
  {
    search_uri = Uri(search_uri_str);
    core_uri = Uri(core_uri_str);
    listen_ports.push_back(core_uri.port);
  }

  std::vector<uint16_t> listen_ports;
  std::size_t comms_thread_count = 10;
  std::size_t tasks_thread_count = 10;

  std::string search_uri_str;
  std::string core_uri_str;
  std::string core_key;

  Uri core_uri;
  Uri search_uri;

    std::map<std::string, std::string> karma_policy;
protected:
private:
  MtCoreArgs(const MtCoreArgs &other) = delete;
  MtCoreArgs &operator=(const MtCoreArgs &other) = delete;
  bool operator==(const MtCoreArgs &other) = delete;
  bool operator<(const MtCoreArgs &other) = delete;
};
