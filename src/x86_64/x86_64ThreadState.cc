//
//  x86_64ThreadState.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64ThreadState.h"
#include <sstream>

const char *thread_registers[] = {"RAX", "RBX", "RCX", "RDX", "RDI", "RSI",
                                  "RBP", "RSP", "R8",  "R9",  "R10", "R11",
                                  "R12", "R13", "R14", "R15", "RIP", "RFLAGS",
                                  "CS",  "FS",  "GS"};

const char *debug_registers[] = {"DR0", "DR1", "DR2", "DR3",
                                 "DR4", "DR5", "DR6", "DR7"};
// potentially unsafe
bool x86_64ThreadState::Load() {
    x86_thread_state64_t &state = this->thread_state;
    mach_msg_type_number_t count = x86_THREAD_STATE64_COUNT;
    thread_get_state(_thread, x86_THREAD_STATE64, (thread_state_t)&state,
                     &count);

    count = x86_DEBUG_STATE64_COUNT;
    thread_get_state(_thread, x86_DEBUG_STATE64,
                     (thread_state_t)&this->debug_state, &count);

    uint64_t *statePtr = (uint64_t *)&state;

    STATE_ADD_REGISTER(this, "RAX",    statePtr + 0);
    STATE_ADD_REGISTER(this, "RBX",    statePtr + 1);
    STATE_ADD_REGISTER(this, "RCX",    statePtr + 2);
    STATE_ADD_REGISTER(this, "RDX",    statePtr + 3);
    STATE_ADD_REGISTER(this, "RDI",    statePtr + 4);
    STATE_ADD_REGISTER(this, "RSI",    statePtr + 5);
    STATE_ADD_REGISTER(this, "RBP",    statePtr + 6);
    STATE_ADD_REGISTER(this, "RSP",    statePtr + 7);
    STATE_ADD_REGISTER(this, "R8",     statePtr + 8);
    STATE_ADD_REGISTER(this, "R9",     statePtr + 9);
    STATE_ADD_REGISTER(this, "R10",    statePtr + 10);
    STATE_ADD_REGISTER(this, "R11",    statePtr + 11);
    STATE_ADD_REGISTER(this, "R12",    statePtr + 12);
    STATE_ADD_REGISTER(this, "R13",    statePtr + 13);
    STATE_ADD_REGISTER(this, "R14",    statePtr + 14);
    STATE_ADD_REGISTER(this, "R15",    statePtr + 15);
    STATE_ADD_REGISTER(this, "RIP",    statePtr + 16);
    STATE_ADD_REGISTER(this, "RFLAGS", statePtr + 17);
    STATE_ADD_REGISTER(this, "CS",     statePtr + 18);
    STATE_ADD_REGISTER(this, "FS",     statePtr + 19);
    STATE_ADD_REGISTER(this, "GS",     statePtr + 20);

    return true;
}

bool x86_64ThreadState::Save() {
    thread_set_state(_thread, x86_THREAD_STATE64,
                     (thread_state_t) & this->thread_state,
                     x86_THREAD_STATE64_COUNT);

    thread_set_state(_thread, x86_DEBUG_STATE64,
                     (thread_state_t) & this->debug_state,
                     x86_DEBUG_STATE64_COUNT);
    return true;
}

std::string x86_64ThreadState::Description() {
    uint64_t *statePtr = (uint64_t *)&this->thread_state;
    std::ostringstream stream;

    for (int i = 0; i < sizeof(x86_thread_state64_t) / sizeof(uint64_t); i++) {
        uint64_t val = statePtr[i];
        stream << thread_registers[i] << ": " << std::dec << val << " ["
               << std::hex << val << "]" << std::endl;
    }
    return stream.str();
}

vm_address_t x86_64ThreadState::CurrentAddress() {
    return thread_state.__rip & ~0x1;
}

ThreadState *ThreadState::ThreadStateFromThread(mach_port_t thread) {
    return new x86_64ThreadState(thread);
}
