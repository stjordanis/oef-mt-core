#include "OefListenerStarterTask.hpp"

#include "mt-core/comms/src/cpp/Oefv1Listener.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"

ExitState OefListenerStarterTask::run(void)
{
  // open port here.
  auto result = std::make_shared<Oefv1Listener>(core, p);

  //when done add to the listeners
  listeners -> add(p, result);

  return ExitState::COMPLETE;
}
