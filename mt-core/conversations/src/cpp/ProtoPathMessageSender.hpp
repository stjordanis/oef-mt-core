#pragma once

#include <vector>
#include <string>

#include "basic_comms/src/cpp/IMessageWriter.hpp"
#include "basic_comms/src/cpp/CharArrayBuffer.hpp"
#include "mt-core/comms/src/cpp/Endianness.hpp"
#include "threading/src/cpp/lib/Waitable.hpp"
#include "threading/src/cpp/lib/Notification.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "basic_comms/src/cpp/EndpointBase.hpp"

#include <list>

namespace google
{
  namespace protobuf
  {
    class Message;
  }
}

class ProtoMessageEndpoint;
class Uri;


class ProtoPathMessageSender
  : public IMessageWriter <std::pair<Uri, std::shared_ptr<google::protobuf::Message>>>
{
public:
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;
  using TXType = std::pair<Uri, std::shared_ptr<google::protobuf::Message>>;
  using consumed_needed_pair = IMessageWriter::consumed_needed_pair;
  using EndpointType         = EndpointBase<std::pair<Uri, std::shared_ptr<google::protobuf::Message>>>;

  static constexpr char const *LOGGING_NAME = "ProtoPathMessageSender";

  ProtoPathMessageSender(std::weak_ptr<EndpointType> endpoint)
  {
    this -> endpoint = endpoint;
  }
  virtual ~ProtoPathMessageSender()
  {
  }

  virtual consumed_needed_pair checkForSpace(const mutable_buffers &data, IMessageWriter::TXQ& txq);
protected:
private:
  Mutex mutex;
  std::weak_ptr<EndpointType> endpoint;


  ProtoPathMessageSender(const ProtoPathMessageSender &other) = delete;
  ProtoPathMessageSender &operator=(const ProtoPathMessageSender &other) = delete;
  bool operator==(const ProtoPathMessageSender &other) = delete;
  bool operator<(const ProtoPathMessageSender &other) = delete;
};
