//
//  x86_64ThreadState.cc
//  ghetto_hook2
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64ThreadState.h"
#include <sstream>

// currently only gpr registers are handled
const char *registers[] = {"RAX", "RBX", "RCX", "RDX",    "RDI", "RSI", "RBP",
                           "RSP", "R8",  "R9",  "R10",    "R11", "R12", "R13",
                           "R14", "R15", "RIP", "RFLAGS", "CS",  "FS",  "GS"};

// potentially unsafe
bool x86_64ThreadState::Load() {
  uint64_t *state = (uint64_t *)_state;

  for (int i = 0; i < sizeof(x86_thread_state64_t) / sizeof(uint64_t); i++) {
    uint64_t *valPtr = state + i;
    _registers.emplace_back(valPtr, registers[i]);
  }

  return true;
}

std::string x86_64ThreadState::Description() {
  std::ostringstream stream;
  uint64_t *state = (uint64_t *)_state;
  if (!state)
    return "";

  for (int i = 0; i < sizeof(x86_thread_state64_t) / sizeof(uint64_t); i++) {
    uint64_t val = state[i];
    stream << registers[i] << ": " << std::dec << val << " [" << std::hex << val
           << "]" << std::endl;
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
  throw std::runtime_error("invalid register called on thread state: " +
                           this->Description() + "\n");
}
