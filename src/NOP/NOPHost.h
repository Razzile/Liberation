//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include <stdexcept>
#include "Host.h"

class NOPHost : public Host {
public:
    int HardwareBreakpointCount() {
        throw std::runtime_error("NOP implementation called");
    }

    int HardwareWatchpointCount() {
        throw std::runtime_error("NOP implementation called");
    }

    enum Platform Platform() { return Platform::UNKNOWN; }
};
