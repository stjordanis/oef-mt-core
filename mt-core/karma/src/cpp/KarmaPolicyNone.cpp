#include "KarmaPolicyNone.hpp"

KarmaPolicyNone::KarmaPolicyNone()
{
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
