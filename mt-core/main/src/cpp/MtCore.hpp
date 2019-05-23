#pragma once


class MtCore
{
public:
  using args = struct
  {
    int tmp;
  };

  MtCore()
  {
  }
  virtual ~MtCore()
  {
  }

  int run(const args &args);
protected:
private:
  MtCore(const MtCore &other) = delete;
  MtCore &operator=(const MtCore &other) = delete;
  bool operator==(const MtCore &other) = delete;
  bool operator<(const MtCore &other) = delete;
};
