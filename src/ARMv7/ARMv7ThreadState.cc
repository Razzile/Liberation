//
//  Liberation
//
//  Copyright © 2016 callum. All rights reserved.
//

#include "ARMv7ThreadState.h"

bool ARMv7ThreadState::Load() {
    mach_msg_type_number_t count;

    count = ARM_THREAD_STATE_COUNT;
    thread_get_state(_thread, ARM_THREAD_STATE, (thread_state_t)&thread_state,
                     &count);

    count = ARM_VFP_STATE_COUNT;
    thread_get_state(_thread, ARM_VFP_STATE, (thread_state_t)&vfp_state,
                     &count);

    count = ARM_NEON_STATE_COUNT;
    thread_get_state(_thread, ARM_NEON_STATE, (thread_state_t)&neon_state,
                     &count);

    count = ARM_EXCEPTION_STATE_COUNT;
    thread_get_state(_thread, ARM_EXCEPTION_STATE,
                     (thread_state_t)&exception_state, &count);

    count = ARM_DEBUG_STATE_COUNT;
    thread_get_state(_thread, ARM_DEBUG_STATE, (thread_state_t)&debug_state,
                     &count);

    uint32_t *threadPtr = (uint32_t *)&thread_state;

    STATE_ADD_REGISTER(this, "R0",   threadPtr + 0);
    STATE_ADD_REGISTER(this, "R1",   threadPtr + 1);
    STATE_ADD_REGISTER(this, "R2",   threadPtr + 2);
    STATE_ADD_REGISTER(this, "R3",   threadPtr + 3);
    STATE_ADD_REGISTER(this, "R4",   threadPtr + 4);
    STATE_ADD_REGISTER(this, "R5",   threadPtr + 5);
    STATE_ADD_REGISTER(this, "R6",   threadPtr + 6);
    STATE_ADD_REGISTER(this, "R7",   threadPtr + 7);
    STATE_ADD_REGISTER(this, "R8",   threadPtr + 8);
    STATE_ADD_REGISTER(this, "R9",   threadPtr + 9);
    STATE_ADD_REGISTER(this, "R10",  threadPtr + 10);
    STATE_ADD_REGISTER(this, "R11",  threadPtr + 11);
    STATE_ADD_REGISTER(this, "R12",  threadPtr + 12);
    STATE_ADD_REGISTER(this, "SP",   threadPtr + 13);
    STATE_ADD_REGISTER(this, "LR",   threadPtr + 14);
    STATE_ADD_REGISTER(this, "PC",   threadPtr + 15);
    STATE_ADD_REGISTER(this, "CPSR", threadPtr + 16);

    return true;
}
