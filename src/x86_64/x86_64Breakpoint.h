//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#pragma once

#include "Breakpoint.h"

#if defined (__i386__) || defined(__x86_64__) // we're targeting an x86 system

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

#else  // we're targeting something else (likely ARM)

#include "NOPBreakpoint.h"
using x86_64HardwareBreakpoint = NOPBreakpoint;
using x86_64SoftBreakpoint = NOPBreakpoint;

#endif /* _x86_64_ */
