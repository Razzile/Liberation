//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64Breakpoint.h"
#include "Host.h"
#include "Process.h"
#include "x86_64ThreadState.h"
#include <thread>

// structure representing the debug register dr7
struct dr7_ctx {
  uint8_t flags_dr0 : 2;
  uint8_t flags_dr1 : 2;
  uint8_t flags_dr2 : 2;
  uint8_t flags_dr3 : 2;

  char unused;

  uint8_t trigger_dr0 : 2;
  uint8_t size_dr0 : 2;
  uint8_t trigger_dr1 : 2;
  uint8_t size_dr1 : 2;
  uint8_t trigger_dr2 : 2;
  uint8_t size_dr2 : 2;
  uint8_t trigger_dr3 : 2;
  uint8_t size_dr3 : 2;

  operator int() { return *(int *)this; }
  void operator=(int val) { memcpy(this, &val, sizeof(int)); }
  dr7_ctx(int val) { memcpy(this, &val, sizeof(int)); }
};

// TODO: MAJOR cleanup of the below code
bool x86_64HardwareBreakpoint::Apply() {
  pthread_t handle = pthread_self();
  mach_port_t mach_thread = pthread_mach_thread_np(handle);
  std::vector<ThreadState *> threads = _proc->Threads(mach_thread);
  for (int i = 0; i < threads.size(); i++) {
    x86_64ThreadState *state = dynamic_cast<x86_64ThreadState *>(threads[i]);
    state->Load();
    x86_debug_state64_t &debug = state->debug_state;
    dr7_ctx dr7 = debug.__dr7;
    switch (in_use) {
    case 0: {
      dr7.flags_dr0 |= 0x1;   // local
      dr7.size_dr0 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr0 |= 0x0; // trigger on code exec

      debug.__dr0 = _address;
      break;
    }

    case 1: {
      dr7.flags_dr1 |= 0x1;   // local
      dr7.size_dr1 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr1 |= 0x0; // trigger on code exec

      debug.__dr1 = _address;
      break;
    }

    case 2: {
      dr7.flags_dr2 |= 0x1;   // local
      dr7.size_dr2 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr2 |= 0x0; // trigger on code exec

      debug.__dr2 = _address;
      break;
    }

    case 3: {
      dr7.flags_dr3 |= 0x1;   // local
      dr7.size_dr3 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr3 |= 0x0; // trigger on code exec

      debug.__dr3 = _address;
      break;
    }

    default: {
      // all debug registers in use
      return false;
    }
    }
    debug.__dr7 = dr7;
    printf("dr7 is %d\n", (int)dr7);
    in_use++;

    state->Save();
  }
  return true;
}

bool x86_64HardwareBreakpoint::Reset() {
  Host *host = Host::CurrentHost();

  pthread_t handle = pthread_self();
  mach_port_t mach_thread = pthread_mach_thread_np(handle);
  std::vector<ThreadState *> threads = _proc->Threads(mach_thread);

  for (int i = 0; i < threads.size(); i++) {
    x86_64ThreadState *state = dynamic_cast<x86_64ThreadState *>(threads[i]);
    state->Load();
    x86_debug_state64_t &debug_state = state->debug_state;

    int index = -1;
    if (debug_state.__dr0 == _address)
      index = 0;
    if (debug_state.__dr1 == _address)
      index = 1;
    if (debug_state.__dr2 == _address)
      index = 2;
    if (debug_state.__dr3 == _address)
      index = 3;

    if (index == -1)
      return false;

    dr7_ctx dr7 = debug_state.__dr7;

    switch (index) {
    case 0: {
      dr7.flags_dr0 = 0;
      break;
    }

    case 1: {
      dr7.flags_dr1 = 0;
      break;
    }

    case 2: {
      dr7.flags_dr2 = 0;
      break;
    }

    case 3: {
      dr7.flags_dr3 = 0;
      break;
    }
    }

    debug_state.__dr7 = dr7;
    state->Save();
  }
  return true;
}

bool x86_64SoftwareBreakpoint::Apply() {
  _active = true;
  static uint8_t opcode[] = {0xCC};

  uint8_t original[1];

  if (_proc->ReadMemory(_address, (char *)original, 1))
    return false;

  _originalOpcode.push_back(*original);

  return _proc->WriteMemory(_address, (char *)opcode, 1, true);
}

bool x86_64SoftwareBreakpoint::Reset() {
  _active = false;
  return _proc->WriteMemory(_address, (char *)_originalOpcode.data(),
                            _originalOpcode.size(), true);
}
