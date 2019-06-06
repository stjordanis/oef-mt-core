#include "KarmaAccount.hpp"

#include "IKarmaPolicy.hpp"

void swap(KarmaAccount& v1, KarmaAccount& v2) {
    v1.swap(v2);
}

bool KarmaAccount::perform(const std::string &action)
{
  return policy -> perform(*this, action);
}

bool KarmaAccount::couldPerform(const std::string &action)
{
  return policy -> couldPerform(*this, action);
}
