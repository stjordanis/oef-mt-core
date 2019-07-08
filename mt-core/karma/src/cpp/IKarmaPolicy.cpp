#include "IKarmaPolicy.hpp"

#include <memory>

#include "threading/src/cpp/lib/Task.hpp"
#include "KarmaRefreshTask.hpp"
#include "KarmaAccount.hpp"

static constexpr std::size_t KARMA_TIME_INTERVAL_MS = 100000;

IKarmaPolicy::IKarmaPolicy()
{
  std::cout << "IKarmaPolicy create.." << std::endl;
  std::shared_ptr<Task> refresher = std::make_shared<KarmaRefreshTask>(this, KARMA_TIME_INTERVAL_MS);
  refresher -> submit();
  std::cout << "IKarmaPolicy created" << std::endl;
}

IKarmaPolicy::~IKarmaPolicy()
{
}



// because friendship is not heritable.
void IKarmaPolicy::changeAccountNumber(KarmaAccount *acc, std::size_t number)
{
  acc -> id = number;
}

// because friendship is not heritable.
KarmaAccount IKarmaPolicy::mkAccount(std::size_t number)
{
  return KarmaAccount(number, this);
}
