#pragma once

#include <utility>
#include <string>
#include <stdexcept>

class IKarmaPolicy;

class KarmaAccount
{
public:
  KarmaAccount(const KarmaAccount &other) { copy(other); }
  KarmaAccount &operator=(const KarmaAccount &other) { copy(other); return *this; }
  bool operator==(const KarmaAccount &other) const { return compare(other)==0; }
  bool operator<(const KarmaAccount &other) const { return compare(other)==-1; }
  KarmaAccount(std::size_t id, IKarmaPolicy *policy)
  {
    this -> id = id;
    this -> policy = policy;
  }
  virtual ~KarmaAccount()
  {
  }

  virtual bool perform(const std::string &action);
  virtual bool couldPerform(const std::string &action);

  friend void swap(KarmaAccount &a, KarmaAccount &b);
protected:

  std::size_t id;
  IKarmaPolicy *policy;

  int compare(const KarmaAccount &other) const
  {
    if (policy != other.policy)
    {
      throw std::logic_error("KarmaAccounts are not comparable between policies.");
    }
    if (id > other.id) return 1;
    if (id < other.id) return -1;
    return 0;
  }
  void copy(const KarmaAccount &other)
  {
    id = other.id;
    policy = other.policy;
  }
  void swap(KarmaAccount &other)
  {
    if (policy != other.policy)
    {
      throw std::logic_error("KarmaAccounts are not comparable between policies.");
    }
    std::swap(id, other.id);
  }
private:
};

void swap(KarmaAccount& v1, KarmaAccount& v2);
