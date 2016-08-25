//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Breakpoint.h"

class ARMv7HardwareBreakpoint : public Breakpoint {
public:
  ARMv7HardwareBreakpoint(Process *proc, vm_address_t addr)
      : Breakpoint(proc, addr) {}

  virtual bool Apply();
  virtual bool Reset();
};

class ARMv7SoftwareBreakpoint : public Breakpoint {
public:
  ARMv7SoftwareBreakpoint(Process *proc, vm_address_t addr)
      : Breakpoint(proc, addr) {}

  virtual bool Apply();
  virtual bool Reset();

protected:
  std::vector<uint8_t> _originalOpcode;
};

class THUMBv7SoftwareBreakpoint : public ARMv7SoftwareBreakpoint {
public:
  THUMBv7SoftwareBreakpoint(Process *proc, vm_address_t addr)
      : ARMv7SoftwareBreakpoint(proc, addr) {}

  virtual bool Apply();
  virtual bool Reset();
};
