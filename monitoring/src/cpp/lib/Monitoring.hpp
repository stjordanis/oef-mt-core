#pragma once

#include <memory>
#include <functional>
#include <string>

class MonitoringInner;

class Monitoring
{
public:
  using IdType = std::size_t;
  using CountType = std::size_t;
  using NameType = std::string;

  using ReportFunc = std::function<void (const std::string &name, std::size_t value)>;

  Monitoring();
  virtual ~Monitoring();

  static IdType find(const NameType &name);
  static void add(IdType id, CountType delta);
  static void sub(IdType id, CountType delta);

  void report(ReportFunc);

  static MonitoringInner *inner;
protected:
private:
  Monitoring(const Monitoring &other) = delete;
  Monitoring &operator=(const Monitoring &other) = delete;
  bool operator==(const Monitoring &other) = delete;
  bool operator<(const Monitoring &other) = delete;
};
