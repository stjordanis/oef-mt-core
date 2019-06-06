#pragma once

#include <string>

class KarmaAccount;

class IKarmaPolicy
{
public:
  IKarmaPolicy();
  virtual ~IKarmaPolicy();

  virtual KarmaAccount getAccount(const std::string &pubkey="", const std::string &ip="") = 0;

  // Returns True or throws
  virtual bool perform(const KarmaAccount &identifier, const std::string &action) = 0;

  virtual bool couldPerform(const KarmaAccount &identifier, const std::string &action) = 0;
protected:
private:
  IKarmaPolicy(const IKarmaPolicy &other) = delete;
  IKarmaPolicy &operator=(const IKarmaPolicy &other) = delete;
  bool operator==(const IKarmaPolicy &other) = delete;
  bool operator<(const IKarmaPolicy &other) = delete;
};

//std::ostream& operator<<(std::ostream& os, const IKarmaPolicy &output) {}
//void swap(IKarmaPolicy& v1, IKarmaPolicy& v2);
