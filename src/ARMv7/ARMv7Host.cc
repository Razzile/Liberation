//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "ARMv7Host.h"

int ARMv7Host::HardwareBreakpointCount() {
#ifdef __arm__
    {
        static int count = -1;
        if (count != -1) return count;

        uint32_t DBGDIDR;
        asm("mrc p14, 0, %0, c0, c0, 0" : "=r"(DBGDIDR));
        int temp = ((DBGDIDR >> 24) & 0xF);
        if (temp > 0) count = temp++;

        return count;
    }
#else
    return 0;  // oops
#endif
}

int ARMv7Host::HardwareWatchpointCount() {
#ifdef __arm__
    {
        static int count = -1;
        if (count != -1) return count;

        uint32_t DBGDIDR;
        asm("mrc p14, 0, %0, c0, c0, 0" : "=r"(DBGDIDR));
        int temp = ((DBGDIDR >> 28) & 0xF);
        if (temp > 0) count = temp++;

        return count;
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
