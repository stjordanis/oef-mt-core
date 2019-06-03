#pragma once

#include "Monitoring.hpp"

class Counter
{
public:
  Counter(const char *name)
  {
    id = Monitoring.find(name);
  }
  virtual ~Counter()
  {
  }
  Counter(const Counter &other) { copy(other); }
  Counter &operator=(const Counter &other) { copy(other); return *this; }

  T& T::operator+=(std::size_t x) { Monitoring.add(id, x); return *this; }
  T& add(std::size_t x) { Monitoring.add(id, x); return *this; }

  T& T::operator++() { Monitoring.add(id, 1); return *this; }
  T T::operator++(int) { Monitoring.add(id, 1); return *this; }
protected:
    std::size_t id;
  void copy(const Counter &other)
  {
    id = other.id;
  }
private:
  bool operator==(const Counter &other) = delete;
  bool operator<(const Counter &other) = delete;
};
