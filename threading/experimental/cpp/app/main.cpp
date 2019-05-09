

#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "threading/src/cpp/lib/Task.hpp"
#include "threading/src/cpp/lib/Threadpool.hpp"

class MyTask:public Task
{
public:
  MyTask(std::size_t id, std::size_t useconds)
  {
    this -> useconds = useconds;
    this -> cancelled = false;
    this -> id = id;
  }

  virtual void collect(void)
  {
    std::cout << "Task "<< id << " collected." << std::endl;
  }

  virtual bool isRunnable(void) const
  {
    return true;
  }
  virtual ExitState run(void) const
  {
    auto useconds = this -> useconds;
    std::cout << "Task "<< id << " completes." << std::endl;
    while(useconds > 0)
    {
      if (cancelled)
      {
        std::cout << "Task "<< id << " cancels." << std::endl;
        return CANCELLED;
      }
      auto naptime = std::min(useconds, std::size_t(1000000));
      std::this_thread::sleep_for(std::chrono::microseconds(naptime));
      useconds -= naptime;
    }

    std::cout << "Task "<< id << " completes." << std::endl;

    return ExitState::COMPLETE;
  }
  virtual void cancel(void)
  {
    this -> cancelled = true;
  }
private:
  bool cancelled;
  std::size_t id;
  std::size_t useconds;

};

int main(int argc, char *argv[])
{
  std::list<std::shared_ptr<MyTask>> myTasks;
  
  for(int i=0;i<100;i++)
  {
    myTasks.push_back(std::make_shared<MyTask>(i, std::rand() % (10 * 1000 * 1000 )));
  }

  Threadpool pool;

  pool.start(20);

  for(auto &t : myTasks)
  {
    pool.post(t);
  }

  for(int i=0; i<7;i++)
  {
    sleep(1);
    auto dones = pool.getFinishedTasks();
    std::cout << "COLLECTING " << dones.size() << " FINISHED TASKS"<<std::endl;
    for(auto& done : dones)
    {
      Task *p = done.second.get();
      MyTask *p2 = (MyTask*)p;
      p2 -> collect();
    }
  }

  for(auto &t : myTasks)
  {
    t -> cancel();
  }

  pool.shutdown();
}
