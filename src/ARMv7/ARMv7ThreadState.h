//
//  Liberation
//
//  Copyright © 2016 callum. All rights reserved.
//

#pragma once

#include <sys/types.h>
#include "ThreadState.h"

#if defined (__arm__) || defined (__arm64__)

class ARMv7ThreadState : public ThreadState {
public:
    ARMv7ThreadState(mach_port_t thread) : ThreadState(thread) {}

    virtual std::string Description() override;
    virtual bool Load() override;
    virtual bool Save() override;
    virtual vm_address_t CurrentAddress() override;

    bool IsThumbMode();

    arm_thread_state_t thread_state;

    // TODO: find out which of these states contains which float reg
    arm_vfp_state_t vfp_state;
    arm_neon_state_t neon_state;

    arm_exception_state_t exception_state;
    arm_debug_state_t debug_state;
};

#else

#include "NOPThreadState.h"
using ARMv7ThreadState = NOPThreadState;

#endif
