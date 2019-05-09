

#include <list>
#include <iostream>
#include <functional>

#include "basic_comms/src/cpp/Endpoint.hpp"
#include "basic_comms/src/cpp/Listener.hpp"
#include "basic_comms/src/cpp/ISocketOwner.hpp"
#include "basic_comms/src/cpp/Core.hpp"
#include "basic_comms/src/cpp/IMessageReader.hpp"
#include "basic_comms/src/cpp/IMessageWriter.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageReader.hpp"
#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"


ISocketOwner *createNewConnection(Core &core)
{
  auto obj = new Endpoint(core, 2000, 2000);

  auto textLineMessageSenderPtr = std::make_shared<TextLineMessageSender>();
  auto textLineMessageReaderPtr = std::make_shared<TextLineMessageReader>(textLineMessageSenderPtr, obj);

  obj -> reader = textLineMessageReaderPtr;
  obj -> writer = textLineMessageSenderPtr;
  return obj;
}

int main(int argc, char *argv[])
{
  Core core;

  Listener listener(core);
  listener.creator = std::bind(createNewConnection, std::ref(core));

  listener.start_accept();

  core.run();
}
