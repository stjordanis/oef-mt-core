#include "KarmaRefreshTask.hpp"

#include <iostream>

#include "IKarmaPolicy.hpp"

ExitState KarmaRefreshTask::run(void)
{
  FETCH_LOG_INFO(LOGGING_NAME, "KarmaRefreshTask RUN");
  std::cout << "KarmaRefreshTask RUN" << std::endl;

  std::chrono::high_resolution_clock::time_point this_execute = std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(this_execute - last_execute);

  policy -> refreshCycle(d);

  submit(std::chrono::milliseconds(10000));
  return COMPLETE;
}
