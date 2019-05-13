#pragma once

#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "basic_comms/src/cpp/CharArrayBuffer.hpp"
#include <vector>
#include <string>

#include "test_text_server/src/protos/message.pb.h"

class ProtoTextLineMessageSender:public IMessageWriter
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  ProtoTextLineMessageSender()
  {
  }
  virtual ~ProtoTextLineMessageSender()
  {
  }

  void send(std::shared_ptr<TextLine> &s)
  {
    Lock lock(mutex);
    txq.push_back(s);
  }

  virtual consumed_needed_pair checkForSpace(const mutable_buffers &data)
  {
    CharArrayBuffer chars(data);
    std::ostream os(&chars);

    std::size_t consumed = 0;
    while(true)
    {
      {
        Lock lock(mutex);
        if (txq.empty())
        {
          break;
        }

        uint32_t body_size = txq.front() -> ByteSize();
        uint32_t head_size = sizeof(uint32_t);
        uint32_t mesg_size = body_size + head_size;
        if (chars.remainingSpace() < mesg_size)
        {
          break;
        }

        chars.write(body_size);
        txq.front() -> SerializeToOstream(&os);
        txq.pop_front();

        consumed += mesg_size;
      }
    }
    return consumed_needed_pair(consumed, 0);
  }
protected:
private:
  Mutex mutex;

  std::list<std::shared_ptr<TextLine>> txq;

  ProtoTextLineMessageSender(const ProtoTextLineMessageSender &other) = delete;
  ProtoTextLineMessageSender &operator=(const ProtoTextLineMessageSender &other) = delete;
  bool operator==(const ProtoTextLineMessageSender &other) = delete;
  bool operator<(const ProtoTextLineMessageSender &other) = delete;
};
