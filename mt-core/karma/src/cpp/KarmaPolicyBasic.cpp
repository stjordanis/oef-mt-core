#include "KarmaPolicyBasic.hpp"



KarmaPolicyBasic::KarmaPolicyBasic(const std::map<std::string, std::string> &config)
{
}

KarmaPolicyBasic::~KarmaPolicyBasic()
{
}

KarmaAccount KarmaPolicyBasic::getAccount(const std::string &pubkey, const std::string &ip)
{
  return KarmaAccount(0, this);
}

bool KarmaPolicyBasic::perform(const KarmaAccount &identifier, const std::string &action)
{
  return true;
}

bool KarmaPolicyBasic::couldPerform(const KarmaAccount &identifier, const std::string &action)
{
  return true;
}
