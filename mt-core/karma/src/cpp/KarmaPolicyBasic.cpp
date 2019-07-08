#include "KarmaPolicyBasic.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "XKarma.hpp"

std::atomic<std::size_t> tick_amounts(0);
std::atomic<std::size_t> tick_counter(0);

static const std::size_t MAX_KARMA = 10000;

KarmaPolicyBasic::Account::Account()
  : karma(MAX_KARMA)
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
    KARMA new_karma = std::min(karma + diff * tick_amounts, MAX_KARMA);

    // Write this as long as nothing has updated karma while we were thinking.
    if (karma.compare_exchange_strong( old_karma, new_karma, std::memory_order_seq_cst))
    {
      when = tc;
      break;
    }
  }
}

std::string KarmaPolicyBasic::getBalance(const KarmaAccount &identifier)
{
  return std::string("account=") + identifier.getName() + "  karma=" +std::to_string(accounts.access(*identifier).karma) + "/" + std::to_string(MAX_KARMA);
}

KarmaPolicyBasic::KarmaPolicyBasic(const google::protobuf::Map<std::string, std::string> &config)
{
    accounts.get("(null karma account)"); // make sure the null account exists.

    for(auto &kv : config)
    {
      this -> config[kv.first] = kv.second;
    }
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
    return mkAccount(getAccountNumber(pubkey), pubkey);
  }
  else if (ip.length() > 0)
  {
    return mkAccount(getAccountNumber(ip), ip);
  }
  else
  {
    return mkAccount(0, "DEFAULT_KARMA_ACCOUNT");
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
    FETCH_LOG_INFO(LOGGING_NAME, "KARMA:  Unknown Karma Event:", action);
    return iter -> second;
  }
  FETCH_LOG_INFO(LOGGING_NAME, "KARMA:  No default karma event:", action);
  return zero;
}

KarmaPolicyBasic::KARMA KarmaPolicyBasic::afterwards(KARMA currentBalance, const std::string &effect)
{
  switch(effect[0])
  {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '+':
  case '-':
    {
      KARMA delta = std::stoi(effect);
      return currentBalance + delta;
    }
  case '=':
    return std::stoi(effect.substr(1));
  default:
    {
      FETCH_LOG_INFO(LOGGING_NAME, "KARMA: Effect which can't be parsed:", effect);
      return currentBalance;
    }
  };
}

bool KarmaPolicyBasic::perform(const KarmaAccount &identifier, const std::string &action, bool force)
{
  accounts.access(*identifier).bringUpToDate();
  KARMA prev = accounts.access(*identifier).karma;
  KARMA next = afterwards(prev, getPolicy(action));

  FETCH_LOG_INFO(LOGGING_NAME, "KARMA: Event ", action, " for ", identifier.getName(), " scores ", prev, " => ",  next);

  if (next >= 0 || force)
  {
    accounts.access(*identifier).karma = next;
    return true;
  }

  throw XKarma(action);
}

bool KarmaPolicyBasic::couldPerform(const KarmaAccount &identifier, const std::string &action)
{
  accounts.access(*identifier).bringUpToDate();
  KARMA prev = accounts.access(*identifier).karma;
  KARMA next = afterwards(prev, getPolicy(action));
  return (next >= 0);
}
