//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#pragma once

#include <sys/types.h>

enum class Platform : unsigned { x86, x86_64, ARMv7, AArch64, UNKNOWN = 0xFFFFFFFF };

class Host {
public:
    static Host *CurrentHost();

    virtual int HardwareBreakpointCount() = 0;
    virtual int HardwareWatchpointCount() = 0;  // unusued in liberation
    virtual Platform Platform() = 0;
};
