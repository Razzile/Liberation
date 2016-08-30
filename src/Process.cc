//
//  Process.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Process.h"
#include "Host.h"
#include "x86_64/x86_64ThreadState.h"
#include <stdlib.h>

#define PROC_ALL_PIDS 1

// cba to include
extern "C" int getpid();
extern "C" int kill(int, int);
extern "C" char ***_NSGetArgv();
extern "C" char **_NSGetProgname();

#define krncall(expr)                                                          \
  do {                                                                         \
    kern_return_t status = (expr);                                             \
    if (status != KERN_SUCCESS)                                                \
      return false;                                                            \
  } while (false)

// XXX: the below non-member functions may not work in an iOS sandbox
// TODO: redo these somewhere better
inline const char *NameForPID(int pid) {
  int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
  size_t sz = numberOfProcesses * sizeof(pid_t);
  pid_t *pids = (pid_t *)malloc(sz);
  proc_listpids(PROC_ALL_PIDS, 0, pids, sz);
  for (int i = 0; i < numberOfProcesses; i++) {
    if (pids[i] != pid)
      continue;
    else if (pids[i] == pid) {
      char *name = new char[64];
      proc_name(pids[i], name, 64);
      free(pids);
      return name;
    }
  }
  free(pids);
  return nullptr;
}

inline int PIDForName(const char *name) {
  int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
  size_t sz = numberOfProcesses * sizeof(pid_t);
  pid_t *pids = (pid_t *)malloc(sz);
  proc_listpids(PROC_ALL_PIDS, 0, pids, sz);
  for (int i = 0; i < numberOfProcesses; i++) {
    char *pid_name = new char[64];
    proc_name(pids[i], pid_name, 64);
    if (strcmp(pid_name, name) == 0) {
      int pid = pids[i];
      free(pids);
      return pid;
    }
  }
  free(pids);
  return -1;
}

inline bool ProcessExists(int pid) {
  int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
  size_t sz = numberOfProcesses * sizeof(pid_t);
  pid_t *pids = (pid_t *)malloc(sz);
  proc_listpids(PROC_ALL_PIDS, 0, pids, sz);
  for (int i = 0; i < numberOfProcesses; i++) {
    if (pids[i] == pid) {
      free(pids);
      return true;
    }
  }
  free(pids);
  return false;
}

ProcessRef Process::GetProcess(const char *name) {
  if (!name)
    return nullptr;
  int pid = PIDForName(name);
  if (pid <= 0)
    return nullptr;

  task_t task;
  task_for_pid(mach_task_self(), pid, &task);
  // Platform plt = PlatformForProcess(pid);

  return std::make_shared<Process>(pid, name, task /*, plt*/);
}

ProcessRef Process::GetProcess(int pid) {
  if (!ProcessExists(pid))
    return nullptr;

  const char *name = NameForPID(pid); // XXX: can be nullptr
  task_t task;
  task_for_pid(mach_task_self(), pid, &task);
  // Platform plt = PlatformForProcess(pid);

  return std::make_shared<Process>(pid, name, task /*, plt*/);
}

ProcessRef Process::Self() {
  const char *name = *_NSGetProgname();
  int pid = getpid();
  task_t task = mach_task_self();
  // Platform plt = PlatformForProcess(pid);

  return std::make_shared<Process>(pid, name, task /*, plt*/);
}

bool Process::IsAlive() { return ProcessExists(this->process_id()); }

bool Process::Kill() { return kill(this->process_id(), SIGKILL); }

// TODO: figure out how to pause our own process without freezing
bool Process::Pause() {
  _paused = true;
  return (task_suspend(this->task()) == KERN_SUCCESS);
}

bool Process::Resume() {
  _paused = false;
  return (task_resume(this->task()) == KERN_SUCCESS);
}

bool Process::InjectLibrary(const char *lib) {
  // XXX: taming this dragon is for another day
  return false;
}

std::vector<ThreadState *> Process::Threads() {
  std::vector<ThreadState *> local;

  Host *host = Host::CurrentHost();

  thread_act_port_array_t threads;
  mach_msg_type_number_t count;
  task_threads(_task, &threads, &count);

  for (int i = 0; i < count; i++) {
    switch (host->Platform()) {
    case Platform::x86_64: {
      local.push_back(new x86_64ThreadState(threads[i]));
      break;
    }
    default:
      break;
    }
  }
  return local;
}

bool Process::ReadMemory(vm_address_t address, char *output, size_t size) {
  vm_size_t sz;
  krncall(vm_read_overwrite(_task, address, size, (vm_address_t)output, &sz));
  return KERN_SUCCESS;
}

bool Process::WriteMemory(vm_address_t address, char *input, size_t size,
                          bool force) {
  if (force) {
    krncall(vm_protect(_task, address, size, false,
                       VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE |
                           VM_PROT_COPY));
    krncall(vm_write(_task, address, (vm_offset_t)input, size));
    // TODO: use vm_region(_xx) to get original prot to restore here
    krncall(vm_protect(_task, address, size, false,
                       VM_PROT_READ | VM_PROT_EXECUTE));
  } else {
    krncall(vm_write(_task, address, (vm_offset_t)input, size));
  }
  return true;
}

std::vector<Process::Region> Process::GetRegions(vm_prot_t options) {
  std::vector<Process::Region> regions;

  kern_return_t status = KERN_SUCCESS;
  vm_address_t address = 0x0;
  uint32_t depth = 1;
  vm_size_t size = 0;

  while (true) {
    struct vm_region_submap_info_xx info;
    mach_msg_type_number_t count = VM_REGION_SUBMAP_INFO_COUNT_XX;
    status = vm_region_recurse_xx(_task, &address, &size, &depth,
                                  (vm_region_info_xx_t)&info, &count);

    if (status == KERN_INVALID_ADDRESS)
      break;

    if (info.is_submap) {
      depth++;
      continue;
    }
    if (info.protection == options) {
      regions.emplace_back(address, size);
    }
    address += size;
  }
  return regions;
}
