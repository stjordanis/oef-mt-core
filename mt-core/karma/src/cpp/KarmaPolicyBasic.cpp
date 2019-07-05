#include "KarmaPolicyBasic.hpp"

#include "XKarma.hpp"

std::atomic<std::size_t> tick_amounts(0);
std::atomic<std::size_t> tick_counter(0);

KarmaPolicyBasic::Account::Account()
  : karma(10000)
  , when(0)
{
}

void KarmaPolicyBasic::Account::bringUpToDate()
{
  while(true)
  {
    KARMA old_karma = karma;
    TICKS tc = tick_counter;
    TICKS diff = tc - when;
    KARMA new_karma = karma + diff * tick_amounts;

    if (karma.compare_exchange_strong( old_karma, new_karma, std::memory_order_seq_cst))
    {
      when = tc;
      break;
    }
  }
}

KarmaPolicyBasic::KarmaPolicyBasic(const std::map<std::string, std::string> &config)
{
  this -> config = config;
}

KarmaPolicyBasic::~KarmaPolicyBasic()
{
}

void KarmaPolicyBasic::refreshTick(std::size_t amount)
{
  tick_amounts = amount;
  tick_counter++;
}

std::size_t KarmaPolicyBasic::getAccountNumber(const std::string &s)
{
  return accounts.get(s);
}

KarmaAccount KarmaPolicyBasic::getAccount(const std::string &pubkey, const std::string &ip)
{
  if (pubkey.length() > 0)
  {
    return mkAccount(getAccountNumber(pubkey));
  }
  else if (ip.length() > 0)
  {
    return mkAccount(getAccountNumber(ip));
  }
  else
  {
    return mkAccount(0);
  }
}

void KarmaPolicyBasic::upgrade(KarmaAccount &account, const std::string &pubkey, const std::string &ip)
{
  auto k = getAccount(pubkey, ip);
  std::swap(account, k);
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

  accounts.access(*identifier).bringUpToDate();
  if (accounts.access(*identifier).karma > delta)
  {
    accounts.access(*identifier).karma -= delta;
    return true;
  }

  throw XKarma(action);
}

bool KarmaPolicyBasic::couldPerform(const KarmaAccount &identifier, const std::string &action)
{
  auto policy = getPolicy(action);
  int delta = std::atoi(policy.c_str());
  accounts.access(*identifier).bringUpToDate();
  return accounts.access(*identifier).karma > delta;
}
