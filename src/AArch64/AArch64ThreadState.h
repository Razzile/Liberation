//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include "ThreadState.h"

#if defined (__arm__) || defined (__arm64__)

class AArch64ThreadState : public ThreadState {
public:
    AArch64ThreadState(mach_port_t port) : ThreadState(port) {}

    virtual std::string Description() override;
    virtual bool Load() override;
    virtual bool Save() override;
    virtual vm_address_t CurrentAddress() override;

    // TODO: arm64 structs
};

#else

#include "NOPThreadState.h"
using AArch64ThreadState = NOPThreadState;

#endif
