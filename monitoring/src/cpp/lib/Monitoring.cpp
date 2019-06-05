#include "Monitoring.hpp"

#include <atomic>
#include <vector>
#include <map>
#include <mutex>
#include <iostream>

class CounterSet
{
public:
  std::size_t size = 0;

  using CounterSubSet = std::vector<std::atomic<std::size_t>>;
  static constexpr std::size_t SUBSET_SIZE = 128;

  std::vector<CounterSubSet*> counterSubSets;

  std::size_t extend()
  {
    if (size >= SUBSET_SIZE * counterSubSets.size())
    {
      auto newCounterSubSet = new CounterSubSet(SUBSET_SIZE);
      counterSubSets.push_back(newCounterSubSet);
    }
    auto r = size++;
    return r;
  }

  std::atomic<std::size_t> &access(std::size_t index)
  {
    CounterSubSet *set = counterSubSets[index/SUBSET_SIZE];
    return (*set)[index%SUBSET_SIZE];
  }
};


class MonitoringInner
{
public:
  std::map<Monitoring::NameType, Monitoring::IdType> names;
  std::mutex mutex;
  CounterSet counterSet;
};

using Lock = std::lock_guard<std::mutex>;
MonitoringInner *Monitoring::inner = 0;

Monitoring::Monitoring()
{
  if (!inner)
  {
    inner = new MonitoringInner;
  }
}

Monitoring::~Monitoring()
{
}


Monitoring::IdType Monitoring::find(const NameType &name)
{
  if (!inner)
  {
    inner = new MonitoringInner;
  }
  Lock lock(inner -> mutex);
  auto iter = inner -> names.find(name);
  if (iter == inner -> names.end())
  {
    auto x = inner -> counterSet.extend();
    inner -> names[name] = x;
    return x;
  }
  
  return iter -> second;
}

void Monitoring::add(IdType id, CountType delta)
{
  if (!Monitoring::inner)
  {
    Monitoring::inner = new MonitoringInner;
  }
  Monitoring::inner -> counterSet.access(id) += delta;
}

void Monitoring::sub(IdType id, CountType delta)
{
  if (!Monitoring::inner)
  {
    Monitoring::inner = new MonitoringInner;
  }
  Monitoring::inner -> counterSet.access(id) -= delta;
}


void Monitoring::report(ReportFunc func)
{
  Lock lock(inner -> mutex);
  for(const auto &name2id : inner -> names)
    {
      func(name2id.first, inner -> counterSet.access(name2id.second));
    }
}
