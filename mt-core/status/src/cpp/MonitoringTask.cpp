#include "MonitoringTask.hpp"

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include "monitoring/src/cpp/lib/Counter.hpp"
#include "monitoring/src/cpp/lib/Gauge.hpp"

struct mem_usage_result
{
  double vsize = 0.0;
  double rss = 0.0;
  bool ok = false;
};

#ifdef __APPLE__

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach_port.h>
#include <mach/mach_traps.h>
#include <mach/task_info.h>
#include <mach/thread_info.h>
#include <mach/thread_act.h>
#include <mach/vm_region.h>
#include <mach/vm_map.h>
#include <mach/task.h>
#include <mach/shared_memory_server.h>
//#include <mach/shared_region.h>

mem_usage_result run_get_dynamic_proc_info(pid_t pid)
{
  mem_usage_result result;

    task_t task;
    kern_return_t error;
    mach_msg_type_number_t count;
    struct task_basic_info ti;

    error = task_for_pid(mach_task_self(), pid, &task);
    if (error != KERN_SUCCESS) {
      return result;
    }
    count = TASK_BASIC_INFO_COUNT;
    error = task_info(task, TASK_BASIC_INFO, (task_info_t)&ti, &count);
    assert(error == KERN_SUCCESS);
    { /* adapted from ps/tasks.c */
        vm_region_basic_info_data_64_t b_info;
        vm_address_t address = GLOBAL_SHARED_TEXT_SEGMENT;
        vm_size_t size;
        mach_port_t object_name;
        count = VM_REGION_BASIC_INFO_COUNT_64;
        error = vm_region_64(task, &address, &size, VM_REGION_BASIC_INFO,
                             (vm_region_info_t)&b_info, &count, &object_name);
        if (error == KERN_SUCCESS) {
            if (b_info.reserved && size == (SHARED_TEXT_REGION_SIZE) &&
                ti.virtual_size > (SHARED_TEXT_REGION_SIZE + SHARED_DATA_REGION_SIZE))
            {
                ti.virtual_size -= (SHARED_TEXT_REGION_SIZE + SHARED_DATA_REGION_SIZE);
            }
        }
        result.rss = ti.resident_size;
        result.vsize = ti.virtual_size;
        result.ok = true;
    }
    return result;
}
#endif

mem_usage_result process_mem_usage(const std::string filename)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   mem_usage_result r;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   if (!stat_stream.is_open())
   {
     return r;
   }

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   r.vsize     = vsize * 1024.0;
   r.rss = rss * page_size_kb * 1024;
   r.ok = true;
   return r;
}

ExitState MonitoringTask::run(void)
{

  mem_usage_result r_self;
  const char *yes, *no;

#ifdef __APPLE__
  yes = "status.macos";
  no = "status.not_macos";
  r_self = run_get_dynamic_proc_info(getpid());
#endif

#ifdef __linux__
  yes = "status.linux";
  no = "status.not_linux";
  r_self = process_mem_usage("/proc/self/stat");
#endif

  if (r_self.ok)
  {
    Gauge marker(yes);
    marker = 1;
    Gauge("mt-core.gauge.vsize") = r_self.vsize;
    Gauge("mt-core.gauge.vsize.mb") = r_self.vsize / 1024 / 1024;
    Gauge("mt-core.gauge.rss") = r_self.rss;
    Gauge("mt-core.gauge.rss.mb") = r_self.rss / 1024 / 1024;
  }
  else
  {
    Counter(no)++;
  }

  submit(std::chrono::milliseconds(10000));
  return COMPLETE;
}
