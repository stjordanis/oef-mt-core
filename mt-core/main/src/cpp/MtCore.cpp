#include "MtCore.hpp"

#include <iostream>

#include "mt-core/comms/src/cpp/OefListenerStarterTask.hpp"
#include "basic_comms/src/cpp/Core.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"

int MtCore::run(const MtCore::args &args)
{
  listeners = std::make_shared<OefListenerSet>();
  core = std::make_shared<Core>();

  startListeners(args.listen_ports);

  std::cout << "hello" << std::endl;
  return 0;
}

void MtCore::startListeners(const std::vector<int> &ports)
{
  for(auto &p : ports)
  {
    auto task = std::make_shared<OefListenerStarterTask>(p, listeners, core);
    task -> submit();
  }
}
