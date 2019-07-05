#pragma once

#include <map>

#include "mt-core/karma/src/cpp/IKarmaPolicy.hpp"
#include "mt-core/karma/src/cpp/KarmaAccount.hpp"
#include "cpp-utils/src/cpp/lib/BucketsOf.hpp"

#include "google/protobuf/map.h"

class KarmaPolicyBasic
  : public IKarmaPolicy
{
public:
  KarmaPolicyBasic(const google::protobuf::Map<std::string, std::string> &config);
  virtual ~KarmaPolicyBasic();

  virtual KarmaAccount getAccount(const std::string &pubkey="", const std::string &ip="") override;
  void upgrade(KarmaAccount &account, const std::string &pubkey="", const std::string &ip="") override;

  virtual bool perform(const KarmaAccount &identifier, const std::string &action) override;
  virtual bool couldPerform(const KarmaAccount &identifier, const std::string &action) override;

  virtual void refreshTick(std::size_t amount);
protected:
private:
  using KARMA = std::size_t;
  using TICKS = std::size_t;

  class Account
  {
  public:
    mutable std::atomic<KARMA> karma;
    mutable std::atomic<TICKS> when;

    Account();

    void bringUpToDate();
  };

  using AccountName = std::string;
  using AccountNumber = std::size_t;
  using Accounts = BucketsOf<Account, std::string, std::size_t, 1024>;
  using Config = std::map<std::string, std::string>;

  Accounts accounts;
  Config config;

  AccountNumber getAccountNumber(const AccountName &s);
  const std::string &getPolicy(const std::string &action) const;

  Account &access(const KarmaAccount &identifier);
  const Account &access(const KarmaAccount &identifier) const;

  KarmaPolicyBasic(const KarmaPolicyBasic &other) = delete;
  KarmaPolicyBasic &operator=(const KarmaPolicyBasic &other) = delete;
  bool operator==(const KarmaPolicyBasic &other) = delete;
  bool operator<(const KarmaPolicyBasic &other) = delete;
};
