#pragma once

#include <vector>
#include <string>

#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "basic_comms/src/cpp/CharArrayBuffer.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "threading/src/cpp/lib/Waitable.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "./basic_comms/src/cpp/Endpoint.hpp"


namespace google
{
  namespace protobuf
  {
    class Message;
  };
};

class ProtoMessageEndpoint;
class Uri;

#include <list>

class ProtoPathMessageSender
  : public IMessageWriter
  , public Waitable
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using consumed_needed_pair = IMessageWriter::consumed_needed_pair;

  using Pb = google::protobuf::Message;
  using PbP = std::shared_ptr<Pb>;
  using Payload = std::pair<Uri, PbP>;
  using Queue = std::list<Payload>;

  static constexpr std::size_t BUFFER_SIZE_LIMIT = 50;
  static constexpr char const *LOGGING_NAME = "ProtoPathMessageSender";

  ProtoPathMessageSender(std::weak_ptr<Endpoint> endpoint)
  {
    this -> endpoint = endpoint;
  }
  virtual ~ProtoPathMessageSender()
  {
  }

  Notification::NotificationBuilder send(std::size_t ident, const Uri &path, std::shared_ptr<google::protobuf::Message> &s);
  virtual consumed_needed_pair checkForSpace(const mutable_buffers &data);
protected:
private:
  Mutex mutex;
  std::weak_ptr<Endpoint> endpoint;
  std::vector<Notification::Notification> waiting;

  Queue txq;

  ProtoPathMessageSender(const ProtoPathMessageSender &other) = delete;
  ProtoPathMessageSender &operator=(const ProtoPathMessageSender &other) = delete;
  bool operator==(const ProtoPathMessageSender &other) = delete;
  bool operator<(const ProtoPathMessageSender &other) = delete;
};
