#include "threading/src/cpp/lib/Taskpool.hpp"

#include "monitoring/src/cpp/lib/Counter.hpp"

Counter created_count("mt-core.tasks.created");
Counter destroyed_count("mt-core.tasks.destroyed");

Task::Task():cancelled(false)
{
  created_count++;
}

Task::~Task()
{
  destroyed_count++;
}

bool Task::submit(std::shared_ptr<Taskpool> pool)
{
  if (this -> pool == pool)
  {
    Counter("mt-core.tasks.submit(pool).not-submitted.aleady-in-pool")++;
    return true;
  }

  if (this -> pool)
  {
    this -> pool -> remove(shared_from_this());
    Counter("mt-core.tasks.submit(pool).remove-from-pool")++;
  }
  this -> pool = pool;
  this -> pool -> submit(shared_from_this());
  return true;
}

bool Task::submit()
{
  auto x = Taskpool::getDefaultTaskpool().lock();
  if (this -> pool == x)
  {
    Counter("mt-core.tasks.submit().not-submitted.aleady-in-pool")++;
    return true;
  }
  if (x)
  {
    return this -> submit( x );
  }
  else
  {
    Counter("mt-core.tasks.submit().not-submitted.no-default-taskpool")++;
  }
  return false;
}

void Task::makeRunnable()
{
  if (this -> pool)
  {
    this -> pool -> makeRunnable(shared_from_this());
  }
  else
  {
    Counter("mt-core.tasks.makeRunnable.no-taskpool")++;
  }
}

bool Task::submit(std::shared_ptr<Taskpool> pool,const std::chrono::milliseconds &delay)
{
  if (this -> pool == pool)
  {
    Counter("mt-core.tasks.submit(pool,delay).not-submitted.aleady-in-pool")++;
    return true;
  }
  if (this -> pool)
  {
    Counter("mt-core.tasks.submit(pool,delay).remove-from-pool")++;
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
  Counter("mt-core.tasks.submit(delay).not-submitted.no-default-taskpool")++;
  return false;
}
