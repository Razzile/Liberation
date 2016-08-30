//
//  x86_64ThreadState.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64ThreadState.h"
#include <sstream>

const char *thread_registers[] = {"RAX", "RBX", "RCX", "RDX", "RDI", "RSI",
                                  "RBP", "RSP", "R8",  "R9",  "R10", "R11",
                                  "R12", "R13", "R14", "R15", "RIP", "RFLAGS",
                                  "CS",  "FS",  "GS"};

const char *debug_registers[] = {"DR0", "DR1", "DR2", "DR3",
                                 "DR4", "DR5", "DR6", "DR7"};
// potentially unsafe
bool x86_64ThreadState::Load() {
  x86_thread_state64_t &state = this->thread_state;
  mach_msg_type_number_t count = x86_THREAD_STATE64_COUNT;
  thread_get_state(_thread, x86_THREAD_STATE64, (thread_state_t)&state, &count);
  uint64_t *statePtr = (uint64_t *)&state;

  for (int i = 0; i < sizeof(x86_thread_state64_t) / sizeof(uint64_t); i++) {
    uint64_t *valPtr = (statePtr + i);
    _registers.emplace_back(valPtr, thread_registers[i]);
  }
  return true;
}

bool x86_64ThreadState::Save() {
  thread_set_state(_thread, x86_THREAD_STATE64,
                   (thread_state_t) & this->thread_state,
                   x86_THREAD_STATE64_COUNT);
  return true;
}

std::string x86_64ThreadState::Description() {
  uint64_t *statePtr = (uint64_t *)&this->thread_state;
  std::ostringstream stream;

  for (int i = 0; i < sizeof(x86_thread_state64_t) / sizeof(uint64_t); i++) {
    uint64_t val = statePtr[i];
    stream << thread_registers[i] << ": " << std::dec << val << " [" << std::hex
           << val << "]" << std::endl;
  }
  return stream.str();
}

ThreadState::Register &x86_64ThreadState::operator[](std::string key) {
  // nasty way to make string uppercase
  for (auto &c : key)
    c = toupper(c);

  for (Register &reg : _registers) {
    if (reg.Name() == key)
      return reg;
  }
  // PLEASE cheaters, don't make this get called
  throw std::runtime_error("invalid register called on thread state: \n" +
                           this->Description() + "\n");
}

vm_address_t x86_64ThreadState::CurrentAddress() {
  return thread_state.__rip & ~0x1;
}
