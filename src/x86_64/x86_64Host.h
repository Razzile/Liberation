//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//
#pragma once
#include "Host.h"

class x86_64Host : public Host {
public:
    virtual int HardwareBreakpointCount();
    virtual int HardwareWatchpointCount();  // unusued in liberation
    virtual enum Platform Platform() { return Platform::x86_64; }
};
