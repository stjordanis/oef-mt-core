#pragma once

#include <string>
#include <iostream>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

class TextLineMessageReader:public IMessageReader
{
public:
  TextLineMessageReader(std::shared_ptr<TextLineMessageSender> sender, std::shared_ptr<Endpoint> endpoint)
  {
    this -> sender = sender;
    this -> endpoint = endpoint;
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
          sender -> send(s);
          endpoint -> run_sending();
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

  std::shared_ptr<TextLineMessageSender> sender;
  std::shared_ptr<Endpoint> endpoint;
protected:
private:
  TextLineMessageReader(const TextLineMessageReader &other) = delete;
  TextLineMessageReader &operator=(const TextLineMessageReader &other) = delete;
  bool operator==(const TextLineMessageReader &other) = delete;
  bool operator<(const TextLineMessageReader &other) = delete;
};
