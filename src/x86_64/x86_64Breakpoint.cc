//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64Breakpoint.h"
#include "Process.h"
#include "x86_64ThreadState.h"

// structure representing the debug register dr7
struct dr7_ctx {
  uint8_t flags_dr0 : 2;
  uint8_t flags_dr1 : 2;
  uint8_t flags_dr2 : 2;
  uint8_t flags_dr3 : 2;

  char unused[8];

  uint8_t trigger_dr0 : 2;
  uint8_t trigger_dr1 : 2;
  uint8_t trigger_dr2 : 2;
  uint8_t trigger_dr3 : 2;

  uint8_t size_dr0 : 2;
  uint8_t size_dr1 : 2;
  uint8_t size_dr2 : 2;
  uint8_t size_dr3 : 2;

  operator int() { return *(int *)this; }
  void operator=(int val) { memcpy(this, &val, sizeof(int)); }
  dr7_ctx(int val) { memcpy(this, &val, sizeof(int)); }
};

// TODO: hardware
bool x86_64HardwareBreakpoint::Apply() {
  std::vector<ThreadState *> threads = _proc->Threads();
  for (int i = 0; i < threads.size(); i++) {
    x86_64ThreadState *state = dynamic_cast<x86_64ThreadState *>(threads[i]);
    x86_debug_state64_t debug = state->debug_state;
    dr7_ctx dr7 = debug.__dr7;
    // TODO: set dr7 flags, trigger and size for each thread
    // TODO: make sure we remain under the Host::count
  }
  return false;
}

bool x86_64HardwareBreakpoint::Reset() { return false; }

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
