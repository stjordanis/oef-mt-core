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
    std::cout << "checkForMessage in " << chars.remainingData() << " bytes." << std::endl;
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

    TransportHeader leader;
    leader . ParseFromIstream(&is);

    consumed += head_size;
    consumed += body_size;

    //std::cout << "MESSAGE = " << head_size << "+" << body_size << " bytes" << std::endl;
    //std::cout << "leader.status.success=" << leader.status().success() << std::endl;
    //std::cout << "leader.status.error_code=" << leader.status().error_code() << std::endl;

    //for(int i=0;i<leader.status().narrative_size();i++)
    //{
    //  std::cout << "leader.status.narrative=" << leader.status().narrative(i) << std::endl;
    //}
    //std::cout << "leader.uri=" <<  leader.uri()<< std::endl;
    //std::cout << "leader.id=" <<  leader.id()<< std::endl;
    if (onComplete)
    {
      FETCH_LOG_WARN(LOGGING_NAME,  "+++++++++++++++++ onComplete.");
      onComplete( leader.status().success(), leader.id(), ConstCharArrayBuffer(chars, chars.current + payload_size));
    }
    else
    {
      FETCH_LOG_WARN(LOGGING_NAME,  "+++++++++++++++++ No onComplete handler set.");
    }
    chars.advance(payload_size);
  }
  return consumed_needed_pair(consumed, needed);
}
