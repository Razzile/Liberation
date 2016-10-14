//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "ARMv7Host.h"

int ARMv7Host::HardwareBreakpointCount() {
#ifdef __arm__
    {
        uint32_t DBGDIDR;
        asm("mrc p14, 0, %0, c0, c0, 0" : "=r"(DBGDIDR));
        int count = ((DBGDIDR >> 24) & 0xF);
        if (count > 0) return count++;
        else return 0;
    }
#else
    return 0;  // oops
#endif
}

int ARMv7Host::HardwareWatchpointCount() {
#ifdef __arm__
    {
        uint32_t DBGDIDR;
        asm("mrc p14, 0, %0, c0, c0, 0" : "=r"(DBGDIDR));
        int count = ((DBGDIDR >> 28) & 0xF);
        if (count > 0) return count++;
        else return 0;
    }
#else
    return 0;  // oops
#endif
}

// TODO: this is ugly, find a cleaner way
#ifdef __arm__

Host *Host::CurrentHost() {
    static Host *host = nullptr;
    if (!host) host = new ARMv7Host();
    return host;
}

#endif
