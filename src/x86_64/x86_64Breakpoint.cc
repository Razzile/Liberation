//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64Breakpoint.h"
#include "Process.h"

// TODO: hardware
bool x86_64HardwareBreakpoint::Apply() { return false; }

bool x86_64HardwareBreakpoint::Reset() { return false; }

bool x86_64SoftwareBreakpoint::Apply() {
  static uint8_t opcode[] = {0xCC};

  uint8_t original[1];

  if (_proc->ReadMemory(_address, (char *)original, 1))
    return false;

  _originalOpcode.push_back(*original);

  return _proc->WriteMemory(_address, (char *)opcode, sizeof(opcode), true);
}

bool x86_64SoftwareBreakpoint::Reset() {
  return _proc->WriteMemory(_address, (char *)_originalOpcode.data(),
                            _originalOpcode.size(), true);
}
