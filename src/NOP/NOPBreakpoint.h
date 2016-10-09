//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include "Breakpoint.h"

class NOPBreakpoint : public Breakpoint {
public:
    virtual bool Apply() {
        throw std::runtime_error("Incorrect Thread State generated");
    };

    virtual bool Reset() {
        throw std::runtime_error("Incorrect Thread State generated");
    };
};
