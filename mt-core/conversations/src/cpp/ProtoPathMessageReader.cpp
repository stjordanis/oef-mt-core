#include "mt-core/conversations/src/cpp/ProtoPathMessageReader.hpp"
#include "mt-core/comms/src/cpp/OefAgentEndpoint.hpp"
#include "protos/src/protos/transport.pb.h"
#include "basic_comms/src/cpp/Endpoint.hpp"


ProtoPathMessageReader::consumed_needed_pair ProtoPathMessageReader::initial() {
  return consumed_needed_pair(0, 1);
}


ProtoPathMessageReader::consumed_needed_pair ProtoPathMessageReader::checkForMessage(const buffers &data)
{
  std::cout << "==================================== ProtoPathMessageReader::checkForMessage" << std::endl;

  std::string s;

  std::size_t consumed = 0;
  std::size_t needed = 1;

  ConstCharArrayBuffer chars(data);
  std::istream is(&chars);

  while(true)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "checkForMessage in ", chars.remainingData(), " bytes. Current: ", chars.current, ", size:", chars.size);
    //chars.diagnostic();

    uint32_t leader_head_size = sizeof(uint32_t);
    uint32_t payload_head_size = sizeof(uint32_t);

    uint32_t head_size = leader_head_size + payload_head_size;

    if (chars.remainingData() < head_size)
    {
      needed = head_size - chars.remainingData();
      break;
    }

    uint32_t leader_size_u32; 
    uint32_t payload_size_u32;

    chars.read(leader_size_u32);
    chars.read(payload_size_u32);

    std::size_t leader_size = leader_size_u32;
    std::size_t payload_size = payload_size_u32;

    std::size_t body_size = leader_size + payload_size;

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

    chars.diagnostic();
    TransportHeader leader;

    auto header_chars = ConstCharArrayBuffer(chars, chars.current + leader_size);
    std::istream h_is(&header_chars);
    if (!leader . ParseFromIstream(&h_is)){
      FETCH_LOG_WARN(LOGGING_NAME, "Failed to parse header!");
      throw std::invalid_argument("Proto deserialization refuses incoming invalid leader message!");
    }
    chars.advance(leader_size);

    consumed += head_size;
    consumed += body_size;

    if (!leader.status().success())
    {
      std::string msg{""};
      for(auto& n : leader.status().narrative()) {
        msg += n;
      }
      int error_code = leader.status().error_code();
      if (error_code == 0) {
        error_code = 132;
      }
      FETCH_LOG_WARN(LOGGING_NAME, "Got error message from search: error_code=",
          error_code, ", message=\"", msg, "\"");
      if (onError)
      {
        onError(leader.id(), error_code, msg);
      }
    }
    else
    {
      if (onComplete)
      {
        FETCH_LOG_WARN(LOGGING_NAME,  "+++++++++++++++++ onComplete.");
        onComplete( leader.status().success(), leader.id(), ConstCharArrayBuffer(chars, chars.current + payload_size));
      }
      else
      {
        FETCH_LOG_WARN(LOGGING_NAME,  "+++++++++++++++++ No onComplete handler set.");
      }
    }

    chars.advance(payload_size);
  }
  return consumed_needed_pair(consumed, needed);
}
