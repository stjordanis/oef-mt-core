#include "KarmaRefreshTask.hpp"

#include <iostream>

#include "IKarmaPolicy.hpp"

ExitState KarmaRefreshTask::run(void)
{

  std::chrono::high_resolution_clock::time_point this_execute = std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(this_execute - last_execute);
  last_execute = std::chrono::high_resolution_clock::now();

  FETCH_LOG_INFO(LOGGING_NAME, "KarmaRefreshTask RUN ms=", d);
  std::cout << "KarmaRefreshTask RUN ms=" << d.count() << std::endl;
  policy -> refreshCycle(d);

  submit(std::chrono::milliseconds(10000));
  return COMPLETE;
}
