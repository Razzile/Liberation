//
//  Breakpoints.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Breakpoint.h"
#include "Process.h"

bool SoftwareBreakpoint::Apply() {
  //   // pause process
  //   std::vector<byte> bkptOpcode;
  //
  //   switch (_proc->platform()) {
  //   case Platform::ARMv7: {
  //     // TODO: check if thumb or ARM
  //     bkptOpcode = {0x00, 0xBE}; // << thumb
  //     // {0x70, 0x00, 0x20, 0xE1} // << armv7
  //     break;
  //   }
  //   case Platform::ARM64: {
  //     bkptOpcode = {0x00, 0x00, 0x20, 0xD4};
  //     break;
  //   }
  //   case Platform::x86:
  //   case Platform::x86_64: {
  //     bkptOpcode = {0xCC};
  //     break;
  //   }
  //   default: { return false; }
  //   }
  //
  //   char orig[bkptOpcode.size()];
  //   if (!_proc->ReadMemory(_address, orig, bkptOpcode.size()))
  //     return false;
  //
  //   if (_proc->WriteMemory(_address, (char *)bkptOpcode.data(),
  //                          bkptOpcode.size())) {
  //     _active = true;
  //   } else {
  //     return false;
  //   }
  //
  //   _active = true;
  return true;
}

bool SoftwareBreakpoint::Reset() {
  // pause process
  if (_proc->WriteMemory(_address, (char *)_originalBytes.data(),
                         _originalBytes.size())) {
    _active = false;
    return true;
  }
  return false;
}

bool HardwareBreakpoint::Apply() {
  // pause process
  // modify debug registers (maybe check if any available)
  // resume process

  return false;
}

bool HardwareBreakpoint::Reset() {
  // pause process
  // modify debug registers (maybe check if any available)
  // resume process

  return false;
}
