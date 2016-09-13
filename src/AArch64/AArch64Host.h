//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Host.h"

class AArch64Host : public Host {
public:
    virtual int HardwareBreakpointCount();
    virtual int HardwareWatchpointCount();  // unusued in liberation
    virtual enum Platform Platform() { return Platform::AArch64; }
};
