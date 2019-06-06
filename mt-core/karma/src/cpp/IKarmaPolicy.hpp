#pragma once

#include <string>
#include <chrono>

class KarmaAccount;

class IKarmaPolicy
{
public:
  static constexpr char const *LOGGING_NAME = "IKarmaPolicy";
  IKarmaPolicy();
  virtual ~IKarmaPolicy();

  virtual KarmaAccount getAccount(const std::string &pubkey="", const std::string &ip="") = 0;

  // Returns True or throws
  virtual bool perform(const KarmaAccount &identifier, const std::string &action) = 0;

  virtual bool couldPerform(const KarmaAccount &identifier, const std::string &action) const = 0;

  virtual void refreshCycle(const std::chrono::milliseconds delta)
  {
  }
protected:
private:
  IKarmaPolicy(const IKarmaPolicy &other) = delete;
  IKarmaPolicy &operator=(const IKarmaPolicy &other) = delete;
  bool operator==(const IKarmaPolicy &other) = delete;
  bool operator<(const IKarmaPolicy &other) = delete;
};
