#include "Monitoring.hpp"

#include <atomic>
#include <vector>

std::vector<std::atomic<Monitoring::CountType>> counts;
std::map<Monitoring::NameType, Monitoring::IdType> names;

std::mutex mutex;

using Lock = std::lock_guard<std::mutex>;

Monitoring::Monitoring()
{
}

Monitoring::~Monitoring()
{
}


IdType Monitoring::find(const NameType &name)
{
  Lock lock(mutex);

  auto iter = names.find(name);
  if (iter == names.end())
  {
    auto x = counts.size();
    names[name] = x;
    counts.push_back(0);
    return x;
  }
  return iter.second;
}

void Monitoring::add(IdType id, CountType delta)
{
  counts[id] += delta;
}

void Monitoring::sub(IdType id, CountType delta)
{
  counts[id] -= delta;
}


void Monitoring::report(std::function<void (const char *name, std::size_t value)> func)
{
  Lock lock(mutex);
  for(const auto &name2id : names)
    {
      func(name2id.first, counts[name2id.second]);
    }
}
