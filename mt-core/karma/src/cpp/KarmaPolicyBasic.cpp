#include "KarmaPolicyBasic.hpp"

#include "XKarma.hpp"

KarmaPolicyBasic::KarmaPolicyBasic(const google::protobuf::Map<std::string, std::string> &config)
{
}

KarmaPolicyBasic::~KarmaPolicyBasic()
{
}

std::size_t KarmaPolicyBasic::getAccountNumber(const std::string &s)
{
  return accounts.get(s);
}

KarmaAccount KarmaPolicyBasic::getAccount(const std::string &pubkey, const std::string &ip)
{
  if (pubkey.length() > 0)
  {
    return KarmaAccount(getAccountNumber(pubkey), this);
  }
  else if (ip.length() > 0)
  {
    return KarmaAccount(getAccountNumber(ip), this);
  }
  else
  {
    return KarmaAccount(0, this);
  }
}

KarmaPolicyBasic::Account &KarmaPolicyBasic::access(const KarmaAccount &identifier)
{
  return accounts.access(*identifier);
}

const KarmaPolicyBasic::Account &KarmaPolicyBasic::access(const KarmaAccount &identifier) const
{
  return accounts.access(*identifier);
}

std::string zero("0");

const std::string &KarmaPolicyBasic::getPolicy(const std::string &action) const
{
  Config::const_iterator iter;
  if ((iter = config.find(action)) != config.end())
  {
    return iter -> second;
  }
  if ((iter = config.find("*")) != config.end())
  {
    return iter -> second;
  }
  return zero;
}

bool KarmaPolicyBasic::perform(const KarmaAccount &identifier, const std::string &action)
{
  auto policy = getPolicy(action);
  int delta = std::atoi(policy.c_str());

  if (accounts.access(*identifier).karma > delta)
  {
    accounts.access(*identifier).karma -= delta;
    return true;
  }

  throw XKarma(action);
}

bool KarmaPolicyBasic::couldPerform(const KarmaAccount &identifier, const std::string &action) const
{
  auto policy = getPolicy(action);
  int delta = std::atoi(policy.c_str());
  return accounts.access(*identifier).karma > delta;
}
