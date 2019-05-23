#pragma once

class IOefAgentTaskFactory
{
public:
  IOefAgentTaskFactory()
  {
  }
  virtual ~IOefAgentTaskFactory()
  {
  }

protected:
private:
  IOefAgentTaskFactory(const IOefAgentTaskFactory &other) = delete;
  IOefAgentTaskFactory &operator=(const IOefAgentTaskFactory &other) = delete;
  bool operator==(const IOefAgentTaskFactory &other) = delete;
  bool operator<(const IOefAgentTaskFactory &other) = delete;

  virtual void processMessage(const google::protobuf::Message &msg) = 0;
  // Process the message, throw exceptions if they're bad.
};
