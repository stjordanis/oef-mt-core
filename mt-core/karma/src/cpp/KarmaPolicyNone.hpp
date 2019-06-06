#pragma once

#include "mt-core/karma/src/cpp/IKarmaPolicy.hpp"
#include "mt-core/karma/src/cpp/KarmaAccount.hpp"

class KarmaPolicyNone
  : public IKarmaPolicy
{
public:
  KarmaPolicyNone();
  virtual ~KarmaPolicyNone();

  virtual KarmaAccount getAccount(const std::string &pubkey="", const std::string &ip="");

  virtual bool perform(const KarmaAccount &identifier, const std::string &action);
  virtual bool couldPerform(const KarmaAccount &identifier, const std::string &action);

protected:
private:
  KarmaPolicyNone(const KarmaPolicyNone &other) = delete;
  KarmaPolicyNone &operator=(const KarmaPolicyNone &other) = delete;
  bool operator==(const KarmaPolicyNone &other) = delete;
  bool operator<(const KarmaPolicyNone &other) = delete;
};
