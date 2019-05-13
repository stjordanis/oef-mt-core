#include "threading/src/cpp/lib/Taskpool.hpp"

bool Task::submit(std::shared_ptr<Taskpool> pool)
{
  if (this -> pool == pool)
  {
    return true;
  }

  if (this -> pool)
  {
    this -> pool -> remove(shared_from_this());
  }
  this -> pool = pool;
  this -> pool -> submit(shared_from_this());
  return true;
}

bool Task::submit()
{
  auto x = Taskpool::getDefaultTaskpool().lock();
  if (x)
  {
    return this -> submit( x );
  }
  return false;
}

void Task::makeRunnable()
{
  if (this -> pool)
  {
    this -> pool -> makeRunnable(shared_from_this());
  }
}

bool Task::submit(std::shared_ptr<Taskpool> pool,const std::chrono::milliseconds &delay)
{
  if (this -> pool == pool)
  {
    return true;
  }

  if (this -> pool)
  {
    this -> pool -> remove(shared_from_this());
  }
  this -> pool = pool;
  this -> pool -> after(shared_from_this(), delay);
  return true;
}

bool Task::submit(const std::chrono::milliseconds &delay)
{
  auto x = Taskpool::getDefaultTaskpool().lock();
  if (x)
  {
    return this -> submit( x, delay );
  }
  return false;
}
