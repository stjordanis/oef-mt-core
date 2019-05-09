#pragma once

#include <string>
#include <iostream>

#include "basic_comms/src/cpp/IMessageReader.hpp"

class TextLineMessageReader:public IMessageReader
{
public:
  TextLineMessageReader()
  {
  }
  virtual ~TextLineMessageReader()
  {
  }

  virtual consumed_needed_pair initial() {
    return consumed_needed_pair(0, 1);
  }

  virtual consumed_needed_pair checkForMessage(const buffers &data, bool consumeMessage=true)
  {
    std::string s;

    std::size_t consumed = 0;
    std::size_t needed = 1;
    std::size_t total_so_far = 0;

    for(auto const& b : data)
    {
      for(unsigned int p = 0; p < b.size(); p++)
      {
        auto cp = (char*)(b.data());
        auto c = cp[p];
        total_so_far += 1;
        if (c != '\n' && c != '\r')
        {
          s += std::string(1, c);
        }
        else
        {
          std::cout << s << std::endl;
          consumed = total_so_far;
          s = "";
        }
      }
    }
    return consumed_needed_pair(consumed, needed);
  }
protected:
private:
  TextLineMessageReader(const TextLineMessageReader &other) = delete;
  TextLineMessageReader &operator=(const TextLineMessageReader &other) = delete;
  bool operator==(const TextLineMessageReader &other) = delete;
  bool operator<(const TextLineMessageReader &other) = delete;
};
