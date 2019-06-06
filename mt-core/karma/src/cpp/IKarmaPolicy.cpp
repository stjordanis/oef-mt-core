#include "IKarmaPolicy.hpp"

#include <memory>

#include "threading/src/cpp/lib/Task.hpp"
#include "KarmaRefreshTask.hpp"

IKarmaPolicy::IKarmaPolicy()
{
  std::cout << "IKarmaPolicy create.." << std::endl;
  std::shared_ptr<Task> refresher = std::make_shared<KarmaRefreshTask>(this);
  refresher -> submit();
  std::cout << "IKarmaPolicy created" << std::endl;
}

IKarmaPolicy::~IKarmaPolicy()
{
}
