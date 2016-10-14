//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include <sys/types.h>
#include "ThreadState.h"

#if defined (__arm__) || defined (__arm64__)

class AArch64ThreadState : public ThreadState {
public:
    AArch64ThreadState(mach_port_t port) : ThreadState(port) {}

    virtual std::string Description() override;
    virtual bool Load() override;
    virtual bool Save() override;
    virtual vm_address_t CurrentAddress() override;

    arm_thread_state64_t thread_state;
    arm_neon_state64_t neon_state;
    arm_exception_state64_t exception_state;
    arm_debug_state64_t debug_state;
};

#else

#include "NOPThreadState.h"
using AArch64ThreadState = NOPThreadState;

#endif
