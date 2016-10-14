//
//  Liberation
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "AArch64ThreadState.h"
#include <sstream>

bool AArch64ThreadState::Load() {
    mach_msg_type_number_t count;

    count = ARM_THREAD_STATE64_COUNT;
    thread_get_state(_thread, ARM_THREAD_STATE64, (thread_state_t)&thread_state,
                     &count);

    count = ARM_NEON_STATE64_COUNT;
    thread_get_state(_thread, ARM_NEON_STATE64, (thread_state_t)&neon_state,
                     &count);

    count = ARM_EXCEPTION_STATE64_COUNT;
    thread_get_state(_thread, ARM_EXCEPTION_STATE64,
                     (thread_state_t)&exception_state, &count);

    count = ARM_DEBUG_STATE64_COUNT;
    thread_get_state(_thread, ARM_DEBUG_STATE64, (thread_state_t)&debug_state,
                     &count);

    // CPSR and PAD are 32 bits
    uint64_t *threadPtr = (uint64_t *)&thread_state;

    STATE_ADD_REGISTER(this, "X0",  threadPtr + 0);
    STATE_ADD_REGISTER(this, "X1",  threadPtr + 1);
    STATE_ADD_REGISTER(this, "X2",  threadPtr + 2);
    STATE_ADD_REGISTER(this, "X3",  threadPtr + 3);
    STATE_ADD_REGISTER(this, "X4",  threadPtr + 4);
    STATE_ADD_REGISTER(this, "X5",  threadPtr + 5);
    STATE_ADD_REGISTER(this, "X6",  threadPtr + 6);
    STATE_ADD_REGISTER(this, "X7",  threadPtr + 7);
    STATE_ADD_REGISTER(this, "X8",  threadPtr + 8);
    STATE_ADD_REGISTER(this, "X9",  threadPtr + 9);
    STATE_ADD_REGISTER(this, "X10", threadPtr + 10);
    STATE_ADD_REGISTER(this, "X11", threadPtr + 11);
    STATE_ADD_REGISTER(this, "X12", threadPtr + 12);
    STATE_ADD_REGISTER(this, "X13", threadPtr + 13);
    STATE_ADD_REGISTER(this, "X14", threadPtr + 14);
    STATE_ADD_REGISTER(this, "X15", threadPtr + 15);
    STATE_ADD_REGISTER(this, "X16", threadPtr + 16);
    STATE_ADD_REGISTER(this, "X17", threadPtr + 17);
    STATE_ADD_REGISTER(this, "X18", threadPtr + 18);
    STATE_ADD_REGISTER(this, "X19", threadPtr + 19);
    STATE_ADD_REGISTER(this, "X20", threadPtr + 20);
    STATE_ADD_REGISTER(this, "X21", threadPtr + 21);
    STATE_ADD_REGISTER(this, "X22", threadPtr + 22);
    STATE_ADD_REGISTER(this, "X23", threadPtr + 23);
    STATE_ADD_REGISTER(this, "X24", threadPtr + 24);
    STATE_ADD_REGISTER(this, "X25", threadPtr + 25);
    STATE_ADD_REGISTER(this, "X26", threadPtr + 26);
    STATE_ADD_REGISTER(this, "X27", threadPtr + 27);
    STATE_ADD_REGISTER(this, "X28", threadPtr + 28);
    STATE_ADD_REGISTER(this, "X29", threadPtr + 29);
    STATE_ADD_REGISTER(this, "X30", threadPtr + 30);
    STATE_ADD_REGISTER(this, "FP",  threadPtr + 31);
    STATE_ADD_REGISTER(this, "LR",  threadPtr + 32);
    STATE_ADD_REGISTER(this, "SP",  threadPtr + 33);
    STATE_ADD_REGISTER(this, "PC",  threadPtr + 34);

    STATE_ADD_REGISTER(this, "CPSR", &thread_state.__cpsr);
    STATE_ADD_REGISTER(this, "PAD",  &thread_state.__pad);

    return true;
}

bool AArch64ThreadState::Save() {
    thread_set_state(_thread, ARM_THREAD_STATE64, (thread_state_t)&thread_state,
                     ARM_THREAD_STATE64_COUNT);
    thread_set_state(_thread, ARM_NEON_STATE64, (thread_state_t)&neon_state,
                     ARM_NEON_STATE64_COUNT);
    thread_set_state(_thread, ARM_EXCEPTION_STATE64,
                     (thread_state_t)&exception_state,
                     ARM_EXCEPTION_STATE64_COUNT);
    thread_set_state(_thread, ARM_DEBUG_STATE64, (thread_state_t)&debug_state,
                     ARM_DEBUG_STATE64_COUNT);

    return true;
}

std::string AArch64ThreadState::Description() {
    std::ostringstream stream;

    for (auto &reg : _registers) {
        uint64_t val;
        if (reg.Name() == "CPSR" || reg.Name() == "PAD") {
            val = reg.Value<uint32_t>();
        }
        else {
            val = reg.Value<uint64_t>();
        }
        stream << reg.Name() << ": " << std::dec << val << " ["
               << std::hex << val << "]" << std::endl;
    }
    return stream.str();
}

vm_address_t AArch64ThreadState::CurrentAddress() {
    return thread_state.__pc;
}
