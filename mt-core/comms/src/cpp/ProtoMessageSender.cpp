#include "ProtoMessageSender.hpp"

#include <google/protobuf/message.h>

ProtoMessageSender::consumed_needed_pair ProtoMessageSender::checkForSpace(const mutable_buffers &data)
{
  CharArrayBuffer chars(data);
  std::ostream os(&chars);

  std::size_t consumed = 0;
  while(true)
  {
    {
      Lock lock(mutex);
      if (txq.size() < BUFFER_SIZE_LIMIT)
      {
        wake();

        if (txq.empty())
        {
          break;
        }
      }

      uint32_t body_size = txq.front() -> ByteSize();
      uint32_t head_size = sizeof(uint32_t);
      uint32_t mesg_size = body_size + head_size;
      if (chars.remainingSpace() < mesg_size)
      {
        std::cout << "out of space on write buffer." << std::endl;
        break;
      }

      switch(endianness)
      {
      case DUNNO:
        throw std::invalid_argument("Refusing to send when endianness is DUNNO.");
        break;
      case LITTLE:
        //std::cout << "send little-endian size " << body_size << std::endl;
        chars.write_little_endian(body_size);
        break;
      case NETWORK:
        //std::cout << "send network-endian size" << body_size << std::endl;
        chars.write(body_size);
        break;
      case BAD:
        throw std::invalid_argument("Refusing to send when endianness is BAD.");
        break;
      }

      txq.front() -> SerializeToOstream(&os);
      txq.pop_front();
      //std::cout << "Ready for sending! bytes=" << mesg_size << std::endl;
      //chars.diagnostic();

      consumed += mesg_size;
    }
  }
  return consumed_needed_pair(consumed, 0);
}

Notification::NotificationBuilder ProtoMessageSender::send(std::shared_ptr<google::protobuf::Message> &s)
{
  Lock lock(mutex);
  if (txq.size() < BUFFER_SIZE_LIMIT)
  {
    txq.push_back(s);
    return Notification::NotificationBuilder();
  }
  else
  {
    return makeNotification();
  }
}

