#include "ProtoPathMessageSender.hpp"

#include <google/protobuf/message.h>
#include "protos/src/protos/transport.pb.h"
#include "cpp-utils/src/cpp/lib/Uri.hpp"

ProtoPathMessageSender::consumed_needed_pair ProtoPathMessageSender::checkForSpace(const mutable_buffers &data)
{
  FETCH_LOG_INFO(LOGGING_NAME, "search message tx...");
  CharArrayBuffer chars(data);
  std::ostream os(&chars);

  std::size_t consumed = 0;
  while(true)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Starting search message tx...");
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

      TransportHeader leader;
      leader.set_uri(txq.front().first.path.substr(1));
      leader.set_id(txq.front().first.port);
      leader.mutable_status() -> set_success(true);
      uint32_t leader_head_size = sizeof(uint32_t);
      uint32_t payload_head_size = sizeof(uint32_t);

      uint32_t payload_size = txq.front().second -> ByteSize();
      uint32_t leader_size = leader.ByteSize();

      uint32_t mesg_size = leader_head_size + leader_size + payload_head_size + payload_size;
      if (chars.remainingSpace() < mesg_size)
      {
        FETCH_LOG_WARN(LOGGING_NAME, "out of space on write buffer.");
        break;
      }

      chars.write(leader_size);
      chars.write(payload_size);
      leader.SerializeToOstream(&os);
      txq.front().second -> SerializeToOstream(&os);

      txq.pop_front();
      //std::cout << "Ready for sending! bytes=" << mesg_size << std::endl;
      //chars.diagnostic();

      consumed += mesg_size;
    }
  }
  return consumed_needed_pair(consumed, 0);
}

Notification::NotificationBuilder ProtoPathMessageSender::send(std::size_t ident, const Uri &path, std::shared_ptr<google::protobuf::Message> &s)
{
  Lock lock(mutex);
  if (txq.size() < BUFFER_SIZE_LIMIT)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "send enqueing");

    Uri uri(path);
    uri.port = ident;
    txq.push_back(std::make_pair(uri, s));
    return Notification::NotificationBuilder();
  }
  else
  {
    return makeNotification();
  }
}

