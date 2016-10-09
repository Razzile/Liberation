//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "Host.h"

#if defined(__x86_64__)  // we're targeting an x86 system

class x86_64Host : public Host {
public:
    virtual int HardwareBreakpointCount();
    virtual int HardwareWatchpointCount();  // unusued in liberation
    virtual enum Platform Platform() { return Platform::x86_64; }
};


#else  // we're targeting something else (likely ARM)

#include "NOPHost.h"
using x86_64Host = NOPHost;

#endif /* _x86_64_ */
