//
//  ThreadState.h
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#ifndef ThreadState_h
#define ThreadState_h

#include <mach/mach.h>
#include <string>
#include <vector>

#define STATE_ADD_REGISTER(state, reg, pointer) \
    state->_registers.emplace_back(pointer, reg)

class ThreadState {
public:
    ThreadState(mach_port_t thread) : _thread(thread) {}
    // nasty wrapper around a register from a thread state
    class Register {
    public:
        Register(void *value, std::string name) : _regVal(value), _name(name) {}

        template <typename U>
        operator U() {
            return *(U *)_regVal;
        }

        template <typename U>
        Register &operator=(U val) {
            *(U *)_regVal = val;
            return *this;
        }

        std::string Name() { return _name; }

    private:
        void *_regVal;
        std::string _name;
    };

    virtual std::string Description() = 0;
    virtual Register &operator[](std::string key) = 0;
    virtual bool Load() = 0;
    virtual bool Save() = 0;
    virtual vm_address_t CurrentAddress() = 0;

protected:
    mach_port_t _thread;
    std::vector<Register> _registers;
};

#endif /* ThreadState_h */
