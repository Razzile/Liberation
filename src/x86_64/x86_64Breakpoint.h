//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Breakpoint.h"

class x86_64HardwareBreakpoint : public Breakpoint {
public:
    x86_64HardwareBreakpoint(Process *proc, vm_address_t addr)
    : Breakpoint(proc, addr) {}

    virtual bool Apply();
    virtual bool Reset();

private:
    int in_use;
};

class x86_64SoftwareBreakpoint : public Breakpoint {
public:
    x86_64SoftwareBreakpoint(Process *proc, vm_address_t addr)
    : Breakpoint(proc, addr) {}

    virtual bool Apply();
    virtual bool Reset();

protected:
    std::vector<uint8_t> _originalOpcode;
};
