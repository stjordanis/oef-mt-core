#include "ProtoMessageReader.hpp"

ProtoMessageReader::consumed_needed_pair ProtoMessageReader::initial() {
    return consumed_needed_pair(0, 1);
  }

ProtoMessageReader::consumed_needed_pair ProtoMessageReader::checkForMessage(const buffers &data)
  {
    std::cout << "ProtoMessageReader::checkForMessage" << std::endl;

    std::string s;

    std::size_t consumed = 0;
    std::size_t needed = 1;

    ConstCharArrayBuffer chars(data);
    std::istream is(&chars);

    while(true)
    {
      std::cout << "checkForMessage in " << chars.remainingData() << " bytes." << std::endl;

      chars.diagnostic();

      uint32_t body_size_u32;
      std::size_t body_size;
      uint32_t head_size = sizeof(uint32_t);

      if (chars.remainingData() < head_size)
      {
        needed = head_size - chars.remainingData();
        break;
      }

      switch(endianness)
      {
      case DUNNO:
        chars.read_little_endian(body_size_u32);
        if (body_size_u32 < 65535)
        {
          std::cout << "detected LITTLE ENDIAN CONNECTION" << std::endl;
          endianness = LITTLE;
        }
        else
        {
          chars.read(body_size_u32);
          if (body_size_u32 < 65535)
          {
            std::cout << "detected NETWORK/BIG ENDIAN CONNECTION" << std::endl;
            endianness = NETWORK;
          }
          else
          {
            throw std::invalid_argument("Could not determine endianness.");
          }
        }
        break;
      case LITTLE:
        chars.read_little_endian(body_size_u32);
        break;
      case NETWORK:
        chars.read(body_size_u32);
        break;
      }

      body_size = body_size_u32;

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

      std::cout << "MESSAGE = " << head_size << "+" << body_size << " bytes" << std::endl;


      if (onComplete)
      {
        std::cout << "COMPLETE MESSAGE" << std::endl;
        onComplete(ConstCharArrayBuffer(chars, chars.current + body_size));
      }
      chars.advance(body_size);
    }
    return consumed_needed_pair(consumed, needed);
  }

