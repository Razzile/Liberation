//
//  Process.h
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#pragma once

#include <mach/mach.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "Host.h"
#include "ThreadState.h"

// TODO: find a vetter way to do this
// TODO: check if vm_region_64 works correctly when targeting 32 bit processes
#ifdef __LP64__  // 64 bit functions

#define vm_region_basic_info_data_xx_t vm_region_basic_info_data_64_t
#define vm_region_info_xx_t vm_region_info_64_t
#define vm_region_xx vm_region_64
#define VM_REGION_BASIC_INFO_XX VM_REGION_BASIC_INFO_64
#define VM_REGION_BASIC_INFO_COUNT_XX VM_REGION_BASIC_INFO_COUNT_64
#define VM_REGION_SUBMAP_INFO_COUNT_XX VM_REGION_SUBMAP_INFO_COUNT_64
#define vm_region_submap_info_xx vm_region_submap_info_64
#define vm_region_recurse_xx vm_region_recurse_64

#else  // 32 bit functions

#define vm_region_basic_info_data_xx_t vm_region_basic_info_data_t
#define vm_region_info_xx_t vm_region_info_t
#define vm_region_xx vm_region
#define VM_REGION_BASIC_INFO_XX VM_REGION_BASIC_INFO
#define VM_REGION_BASIC_INFO_COUNT_XX VM_REGION_BASIC_INFO_COUNT
#define VM_REGION_SUBMAP_INFO_COUNT_XX VM_REGION_SUBMAP_INFO_COUNT
#define vm_region_submap_info_xx vm_region_submap_info
#define vm_region_recurse_xx vm_region_recurse

#endif

extern "C" int proc_listpids(uint32_t type, uint32_t typeinfo, void *buffer,
                             int buffersize);
extern "C" int proc_pidpath(int pid, void *buffer, uint32_t buffersize);
extern "C" int proc_name(int pid, void *buffer, uint32_t buffersize);

class Process;

using ProcessRef = std::shared_ptr<Process>;

class Process {
public:
    struct Region {
        vm_address_t start;
        size_t size;
        Region(vm_address_t start, size_t size) : start(start), size(size) {}
    };

    static ProcessRef GetProcess(const char *name);
    static ProcessRef GetProcess(int pid);
    static ProcessRef Self();

    Process(int pid, const char *name, task_t task)
    : _pid(pid), _name(name), _task(task), _paused(false) {}

    bool IsAlive();
    bool Kill();

    bool Pause();
    bool Resume();

    bool InjectLibrary(const char *lib);  // TODO: add this at later date

    enum Platform RunningPlatform();

    // can ref values be used with virtual classes?
    std::vector<ThreadState *> Threads(
        mach_port_t ignore = 0);  // TODO: return empty when not paused

    bool ReadMemory(vm_address_t address, char *output, size_t size);
    bool WriteMemory(vm_address_t address, char *input, size_t size,
                     bool force = false);
    std::vector<Process::Region> GetRegions(vm_prot_t options = VM_PROT_READ |
                                                                VM_PROT_WRITE);

    pid_t process_id() { return _pid; }
    std::string name() { return _name; }
    task_t task() { return _task; }
    bool paused() { return _paused; }

private:
    pid_t _pid;
    std::string _name;
    task_t _task;
    bool _paused;
};
