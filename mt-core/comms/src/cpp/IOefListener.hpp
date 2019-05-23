#pragma once

#include <functional>
#include <memory>

class IOefAgentTaskFactory;
class OefAgentEndpoint;

class IOefListener
{
public:
  using FactoryCreator = std::function<std::shared_ptr<IOefAgentTaskFactory> (std::shared_ptr<OefAgentEndpoint>)>;

  IOefListener()
  {
  }
  virtual ~IOefListener()
  {
  }

  FactoryCreator factoryCreator;

protected:
private:
  IOefListener(const IOefListener &other) = delete;
  IOefListener &operator=(const IOefListener &other) = delete;
  bool operator==(const IOefListener &other) = delete;
  bool operator<(const IOefListener &other) = delete;
};
