#pragma once

#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "basic_comms/src/cpp/CharArrayBuffer.hpp"
#include <vector>
#include <string>

class TextLineMessageSender:public IMessageWriter<std::string>
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  TextLineMessageSender()
  {
  }
  virtual ~TextLineMessageSender()
  {
  }

  void send(const std::string &s)
  {
    Lock lock(mutex);
    txq.push_back(s);
  }

  virtual consumed_needed_pair checkForSpace(const mutable_buffers &data)
  {
    CharArrayBuffer chars(data);
    std::size_t consumed = 0;
    while(true)
    {
      {
        Lock lock(mutex);
        if (txq.empty())
        {
          break;
        }
      }

      std::string s;
      {
        Lock lock(mutex);
        s = txq.front();
      }

      if (chars.remainingSpace() < s.size()+1 )
      {
        break;
      }

      {
        Lock lock(mutex);
        txq.pop_front();
      }

      for(std::size_t i=0;i<s.size();i++)
      {
        chars.xsputn(s.c_str(), s.size());
        chars.sputc('\n');
      }

      consumed += s.size()+1;
    }

    return consumed_needed_pair(consumed, 0);
  }
protected:
private:
  Mutex mutex;

  std::list<std::string> txq;

  TextLineMessageSender(const TextLineMessageSender &other) = delete;
  TextLineMessageSender &operator=(const TextLineMessageSender &other) = delete;
  bool operator==(const TextLineMessageSender &other) = delete;
  bool operator<(const TextLineMessageSender &other) = delete;
};
