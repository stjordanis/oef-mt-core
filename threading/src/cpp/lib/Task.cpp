#include "threading/src/cpp/lib/Taskpool.hpp"

#include "monitoring/src/cpp/lib/Counter.hpp"

Counter created_count("mt-core.tasks.created");
Counter destroyed_count("mt-core.tasks.created");
Counter remove_count("mt-core.tasks.removed");
Counter submit_count("mt-core.tasks.submitted");
Counter set_defer_count("mt-core.tasks.set_defer");
Counter set_run_count("mt-core.tasks.set_runnable");

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
    return true;
  }

  if (this -> pool)
  {
    this -> pool -> remove(shared_from_this());
    remove_count++;
  }
  submit_count++;
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
  set_run_count++;
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
