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
        throw std::runtime_error("Incorrect Thread State generated");
    }

    int HardwareWatchpointCount() {
        throw std::runtime_error("Incorrect Thread State generated");
    }

    enum Platform Platform() { return Platform::UNKNOWN; }
};
