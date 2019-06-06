#pragma once

#include <map>

#include "mt-core/karma/src/cpp/IKarmaPolicy.hpp"
#include "mt-core/karma/src/cpp/KarmaAccount.hpp"

class KarmaPolicyBasic
  : public IKarmaPolicy
{
public:
  KarmaPolicyBasic(const std::map<std::string, std::string> &config);
  virtual ~KarmaPolicyBasic();

  virtual KarmaAccount getAccount(const std::string &pubkey="", const std::string &ip="");

  virtual bool perform(const KarmaAccount &identifier, const std::string &action);
  virtual bool couldPerform(const KarmaAccount &identifier, const std::string &action);
protected:
private:
  KarmaPolicyBasic(const KarmaPolicyBasic &other) = delete;
  KarmaPolicyBasic &operator=(const KarmaPolicyBasic &other) = delete;
  bool operator==(const KarmaPolicyBasic &other) = delete;
  bool operator<(const KarmaPolicyBasic &other) = delete;
};
