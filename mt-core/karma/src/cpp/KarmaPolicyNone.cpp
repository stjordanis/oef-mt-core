#include "KarmaPolicyNone.hpp"

#include <unistd.h>

KarmaPolicyNone::KarmaPolicyNone(int)
  : IKarmaPolicy()
{
  FETCH_LOG_INFO(LOGGING_NAME, "KarmaPolicyNone created");
  std::cout << "KarmaPolicyNone created" << std::endl;
  sleep(2);
}

KarmaPolicyNone::~KarmaPolicyNone()
{
}

KarmaAccount KarmaPolicyNone::getAccount(const std::string &pubkey, const std::string &ip)
{
  return KarmaAccount(0, this);
}

bool KarmaPolicyNone::perform(const KarmaAccount &identifier, const std::string &action)
{
  return true;
}

bool KarmaPolicyNone::couldPerform(const KarmaAccount &identifier, const std::string &action)
{
  return true;
}
