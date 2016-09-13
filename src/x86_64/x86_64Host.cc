//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64Host.h"

int x86_64Host::HardwareBreakpointCount() {
    return 4;  // always 4
}

int x86_64Host::HardwareWatchpointCount() {
    return 4;  // always 4
}

#ifdef __x86_64__

Host *Host::CurrentHost() {
    static Host *host = nullptr;
    if (!host) host = new x86_64Host();
    return host;
}

#endif
