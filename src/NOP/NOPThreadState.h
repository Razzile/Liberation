//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include "ThreadState.h"

class NOPThreadState : public ThreadState {
public:
    NOPThreadState(mach_port_t thread) : ThreadState(thread) {}

    virtual std::string Description() {
        throw std::runtime_error("Incorrect Thread State generated");
    };

    virtual bool Load() {
        throw std::runtime_error("Incorrect Thread State generated");
    };

    virtual bool Save() {
        throw std::runtime_error("Incorrect Thread State generated");
    };

    virtual vm_address_t CurrentAddress() {
        throw std::runtime_error("Incorrect Thread State generated");
    };
};

#endif
