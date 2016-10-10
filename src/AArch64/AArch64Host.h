//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#pragma once

#include "Host.h"

#if defined (__arm__) || defined (__arm64__)

class AArch64Host : public Host {
public:
    virtual int HardwareBreakpointCount();
    virtual int HardwareWatchpointCount();  // unusued in liberation
    virtual enum Platform Platform() { return Platform::AArch64; }
};

#else

#include "NOPHost.h"
using AArch64Host = NOPHost;

#endif
