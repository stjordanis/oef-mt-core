#include "Uri.hpp"

#include <iostream>
#include <iterator>
#include <string>
#include <regex>

using ParseFormat = struct {
  const char *r;
  int proto;
  int host;
  int port;
  int path;
};

ParseFormat formats[] = {
  { "^([-a-zA-Z0-9\\.]+):([0-9]+)$", 0, 2, 3, 0 },
  { "^([a-z0-9A-Z]+)://([-a-zA-Z0-9\\.]+):([0-9]+)$", 1, 2, 3, 0 },
  { "^([-a-zA-Z0-9\\.]+):([0-9]+)(/.*)$", 0, 2, 3, 4 },
  { "^([a-z0-9A-Z]+)://([-a-zA-Z0-9\\.]+):([0-9]+)(/.*)", 1, 2, 3, 4 },
  { 0, 0, 0, 0, 0 }
};


void Uri::parse(const std::string &s)
{
  ParseFormat *format = formats;

  while(format -> r)
  {
    std::regex uri_regex(format -> r, std::regex_constants::icase);

    valid = false;
    proto = "";
    host = "";
    path = "";
    port = 0;

    std::smatch parts_result;
    if (std::regex_match(s, parts_result, uri_regex)) {
      std::vector<std::sub_match<std::string::const_iterator>> v(parts_result.begin(), parts_result.end());

      if (format -> proto) { proto = v[format -> proto]; }
      if (format -> host)  { host  = v[format -> host]; }
      if (format -> port)  { port  = std::stoi( v[format -> port] ); }
      if (format -> path)  { path  = v[format -> path]; }

      valid = true;
      return;
    }
    format++;
  }
}
