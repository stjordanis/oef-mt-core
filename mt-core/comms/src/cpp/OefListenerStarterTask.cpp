#include "OefListenerStarterTask.hpp"

#include <iostream>

#include "mt-core/comms/src/cpp/Oefv1Listener.hpp"
#include "mt-core/comms/src/cpp/OefListenerSet.hpp"

ExitState OefListenerStarterTask::run(void)
{
  // open port here.
  std::cout << "Listener on "<< p << "make.." << std::endl;
  auto result = std::make_shared<Oefv1Listener>(core, p);

  result -> factoryCreator = initialFactoryCreator;

  result -> start();

  //when done add to the listeners
  listeners -> add(p, result);
  return ExitState::COMPLETE;
}
