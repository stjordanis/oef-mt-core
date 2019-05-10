#include "test_text_server/src/cpp/lib/ChatEndpoint.hpp"
#include "test_text_server/src/cpp/lib/BroadcastTask.hpp"
#include "test_text_server/src/cpp/lib/EndpointCollection.hpp"

#include "test_text_server/src/cpp/lib/TextLineMessageSender.hpp"

void BroadcastTask::broadcast()
{
  ec -> visit(
              [this](std::shared_ptr<ChatEndpoint> ep){
                if (ep -> id != from)
                {
                  ep -> textLineMessageSender -> send(s);
                  ep -> run_sending();
                }
              }
              );
}
