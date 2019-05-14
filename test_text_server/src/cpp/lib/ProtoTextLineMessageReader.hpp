#pragma once

#include <string>
#include <iostream>

#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"
#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"
#include "test_text_server/src/protos/message.pb.h"

class ProtoTextLineMessageReader:public IMessageReader
{
public:
  using Data = std::shared_ptr<TextLine>;
  using CompleteNotification = std::function<void (Data)>;

  ProtoTextLineMessageReader()
  {
  }
  virtual ~ProtoTextLineMessageReader()
  {
  }

  virtual consumed_needed_pair initial() {
    return consumed_needed_pair(0, 1);
  }

  virtual consumed_needed_pair checkForMessage(const buffers &data)
  {
    //std::cout << "ProtoTextLineMessageReader::checkForMessage" << std::endl;

    std::string s;

    std::size_t consumed = 0;
    std::size_t needed = 1;

    ConstCharArrayBuffer chars(data);
    std::istream is(&chars);

    while(true)
    {
      //std::cout << "checkForMessage in " << chars.remainingData() << " bytes." << std::endl;

      uint32_t body_size;
      uint32_t head_size = sizeof(uint32_t);

      if (chars.remainingData() < head_size)
      {
        needed = head_size - chars.remainingData();
        break;
      }

      chars.read(body_size);

      if (body_size > 10000) // TODO(kll)
      {
        throw std::invalid_argument(
                             std::string("Proto deserialisation refuses incoming ")
                             + std::to_string(body_size)
                             + " bytes message header.");
        break;
      }

      if (chars.remainingData() < body_size)
      {
        needed = body_size - chars.remainingData();
        break;
      }

      consumed += head_size;
      consumed += body_size;

      //std::cout << "MESSAGE = " << head_size << "+" << body_size << " bytes" << std::endl;

      auto current = chars.remainingData();
      Data data = std::make_shared<TextLine>();
      auto result = data -> ParseFromIstream(&is);
      auto eaten = current - chars.remainingData();

      if (!result)
      {
        throw std::invalid_argument("Failed proto deserialisation.");
      }
      if (eaten != body_size)
      {
        throw std::invalid_argument(
                             std::string("Proto deserialisation used ")
                             + std::to_string(eaten)
                             + " bytes instead of "
                             + std::to_string(body_size)
                             + ".");
      }

      if (onComplete)
      {
        //std::cout << "COMPLETE MESSAGE" << std::endl;
        onComplete(data);
      }
    }
    return consumed_needed_pair(consumed, needed);
  }

  CompleteNotification onComplete;
protected:
private:
  ProtoTextLineMessageReader(const ProtoTextLineMessageReader &other) = delete;
  ProtoTextLineMessageReader &operator=(const ProtoTextLineMessageReader &other) = delete;
  bool operator==(const ProtoTextLineMessageReader &other) = delete;
  bool operator<(const ProtoTextLineMessageReader &other) = delete;
};
