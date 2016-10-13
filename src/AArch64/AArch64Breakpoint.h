//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#pragma once

#include "Breakpoint.h"

#if defined (__arm__) || defined (__arm64__)

class AArch64HardwareBreakpoint : public Breakpoint {
public:
    AArch64HardwareBreakpoint(Process *proc, vm_address_t addr)
    : Breakpoint(proc, addr) {}

    virtual bool Apply();
    virtual bool Reset();
};

class AArch64SoftwareBreakpoint : public Breakpoint {
public:
    AArch64SoftwareBreakpoint(Process *proc, vm_address_t addr)
    : Breakpoint(proc, addr) {}

    virtual bool Apply();
    virtual bool Reset();

protected:
    std::vector<uint8_t> _originalOpcode;
};

#else

#include "NOPBreakpoint.h"
using AArch64HardwareBreakpoint = NOPBreakpoint;
using AArch64SoftwareBreakpoint = NOPBreakpoint;

#endif
