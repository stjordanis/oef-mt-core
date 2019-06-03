#pragma once

#include <memory>
#include <functional>
#include <string>

class Monitoring
{
public:
  using IdType = std::size_t;
  using CountType = std::size_t;
  using NameType = std::string;

  Monitoring();
  virtual ~Monitoring();

  IdType find(const NameType &name);
  void add(IdType id, CountType delta);
  void sub(IdType id, CountType delta);

  void report(std::function<void (const NameType &name, const CountType &value)>);
protected:
private:
  Monitoring(const Monitoring &other) = delete;
  Monitoring &operator=(const Monitoring &other) = delete;
  bool operator==(const Monitoring &other) = delete;
  bool operator<(const Monitoring &other) = delete;
};
