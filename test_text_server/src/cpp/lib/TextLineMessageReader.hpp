#pragma once

#include <string>
#include <iostream>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

class TextLineMessageReader:public IMessageReader
{
public:
  using CompleteNotification = std::function<void (const std::string &)>;

  TextLineMessageReader()
  {
  }
  virtual ~TextLineMessageReader()
  {
  }

  virtual consumed_needed_pair initial() {
    return consumed_needed_pair(0, 1);
  }

  virtual consumed_needed_pair checkForMessage(const buffers &data)
  {
    std::string s;

    std::size_t consumed = 0;
    std::size_t needed = 1;
    std::size_t total_so_far = 0;

    for(auto const& b : data)
    {
      char prev = 0;
      for(unsigned int p = 0; p < b.size(); p++)
      {
        auto cp = (char*)(b.data());
        auto c = cp[p];
        total_so_far += 1;

        switch(c)
        {
        case '\r':
          if (onComplete)
          {
            onComplete(s);
          }
          consumed = total_so_far;
          s = "";

        case '\n':
          break;
        default:
          s += std::string(1, c);
          break;
        }

        prev = c;
      }
    }
    return consumed_needed_pair(consumed, needed);
  }

  CompleteNotification onComplete;
protected:
private:
  TextLineMessageReader(const TextLineMessageReader &other) = delete;
  TextLineMessageReader &operator=(const TextLineMessageReader &other) = delete;
  bool operator==(const TextLineMessageReader &other) = delete;
  bool operator<(const TextLineMessageReader &other) = delete;
};
