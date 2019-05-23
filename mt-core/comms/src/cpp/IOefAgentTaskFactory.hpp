#pragma once

#include <boost/asio.hpp>
#include <vector>

#include "basic_comms/src/cpp/ConstCharArrayBuffer.hpp"

class IOefAgentTaskFactory
{
public:
  using buffer = boost::asio::const_buffer;
  using buffers = std::vector<buffer>;

  IOefAgentTaskFactory()
  {
  }
  virtual ~IOefAgentTaskFactory()
  {
  }

  virtual void processMessage(const buffers &data) = 0;
  // Process the message, throw exceptions if they're bad.
protected:
private:
  IOefAgentTaskFactory(const IOefAgentTaskFactory &other) = delete;
  IOefAgentTaskFactory &operator=(const IOefAgentTaskFactory &other) = delete;
  bool operator==(const IOefAgentTaskFactory &other) = delete;
  bool operator<(const IOefAgentTaskFactory &other) = delete;

};
