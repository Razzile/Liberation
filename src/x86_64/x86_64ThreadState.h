//
//  x86_64ThreadState.h
//  ghetto_hook2
//
//  Copyright © 2016 Satori. All rights reserved.
//

#ifndef _x86_64ThreadState_
#define _x86_64ThreadState_

#include "ThreadState.h"

extern const char *thread_registers[];
extern const char *debug_registers[];

class x86_64ThreadState : public ThreadState {
public:
    x86_64ThreadState(mach_port_t thread) : ThreadState(thread) {}

    virtual std::string Description() override;
    virtual Register &operator[](std::string key) override;
    virtual bool Load() override;
    virtual bool Save() override;
    virtual vm_address_t CurrentAddress() override;

    x86_thread_state64_t thread_state;
    x86_float_state64_t float_state;
    x86_debug_state64_t debug_state;
    x86_exception_state64_t exception_state;
};

#endif /* _x86_64ThreadState_ */
