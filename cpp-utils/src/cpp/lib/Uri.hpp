#pragma once

#include <string>
#include <iostream>

class Uri
{
public:
  Uri(const std::string &s)
  {
    this -> s = s;
    parse(s);
  }

  Uri(const Uri &other) { copy(other); }
  Uri &operator=(const Uri &other) { copy(other); return *this; }

  virtual ~Uri()
  {
  }

  std::string s;
  std::string proto;
  std::string host;
  std::string path;
  int port;
  bool valid;

  void diagnostic()
  {
    std::cout
      << "valid=" << valid
      << " proto=\"" << proto
      << "\" host=\"" << host
      << "\" port=" << port
      << " path=\"" << path
      << "\"" << std::endl;
  }

protected:
  void parse(const std::string &s);

  void copy(const Uri &other)
  {
    s = other.s;
    valid = other.valid;
    proto = other.proto;
    host = other.host;
    port = other.port;
    path = other.path;
  }
private:
  bool operator==(const Uri &other) = delete; // const { return compare(other)==0; }
  bool operator<(const Uri &other) = delete; // const { return compare(other)==-1; }
};
