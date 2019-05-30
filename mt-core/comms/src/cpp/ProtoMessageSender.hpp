#pragma once

#include <vector>
#include <string>

#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "basic_comms/src/cpp/CharArrayBuffer.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "mt-core/threading/src/cpp/Waitable.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "fetch_teams/ledger/logger.hpp"

namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class ProtoMessageEndpoint;

class ProtoMessageSender
  : public IMessageWriter
  , public Waitable
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using consumed_needed_pair = IMessageWriter::consumed_needed_pair;

  static constexpr std::size_t BUFFER_SIZE_LIMIT = 50;
  static constexpr char const *LOGGING_NAME = "ProtoMessageSender";

  ProtoMessageSender(std::weak_ptr<ProtoMessageEndpoint> &endpoint)
  {
    this -> endpoint = endpoint;
  }
  virtual ~ProtoMessageSender()
  {
  }

  Notification::NotificationBuilder send(std::shared_ptr<google::protobuf::Message> &s);
  virtual consumed_needed_pair checkForSpace(const mutable_buffers &data);
  void setEndianness(Endianness newstate) { endianness = newstate; }
protected:
private:
  Mutex mutex;
  Endianness endianness = DUNNO;
  std::weak_ptr<ProtoMessageEndpoint> endpoint;
  std::vector<Notification::Notification> waiting;

  std::list<std::shared_ptr<google::protobuf::Message>> txq;

  ProtoMessageSender(const ProtoMessageSender &other) = delete;
  ProtoMessageSender &operator=(const ProtoMessageSender &other) = delete;
  bool operator==(const ProtoMessageSender &other) = delete;
  bool operator<(const ProtoMessageSender &other) = delete;
};
