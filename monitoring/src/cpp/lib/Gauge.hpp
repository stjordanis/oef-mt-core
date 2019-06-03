#pragma once

#include "Monitoring.hpp"

class Gauge
{
public:
  Gauge(const char *name)
  {
    id = Monitoring.find(name);
  }
  virtual ~Gauge()
  {
  }
  Gauge(const Gauge &other) { copy(other); }
  Gauge &operator=(const Gauge &other) { copy(other); return *this; }

  T& T::operator+=(std::size_t x) { Monitoring.add(id, x); return *this; }
  T& add(std::size_t x) { Monitoring.add(id, x); return *this; }

  T& T::operator-=(std::size_t x) { Monitoring.sub(id, x); return *this; }
  T& sub(std::size_t x) { Monitoring.sub(id, x); return *this; }

  T& T::operator++() { Monitoring.add(id, 1); return *this; }
  T T::operator++(int) { Monitoring.add(id, 1); return *this; }
  T& T::operator--() { Monitoring.sub(id, 1); return *this; }
  T T::operator--(int) { Monitoring.sub(id, 1); return *this; }
protected:
    std::size_t id;
  void copy(const Gauge &other)
  {
    id = other.id;
  }
private:
  bool operator==(const Gauge &other) = delete;
  bool operator<(const Gauge &other) = delete;
};
