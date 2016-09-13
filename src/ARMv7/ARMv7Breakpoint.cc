//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "ARMv7Breakpoint.h"
#include "Process.h"
#include "ThreadState.h"

bool ARMv7HardwareBreakpoint::Apply() {
    // if (HardwareSlotsLeft()) {
    //   ARMv7ThreadState &state = proc->Threads()[0];
    // }
    return false;
}

bool ARMv7HardwareBreakpoint::Reset() {
    return false;
}

bool ARMv7SoftwareBreakpoint::Apply() {
    // may need to reverse endian
    static uint8_t opcode[] = {0x70, 0x00, 0x20, 0xE1};

    uint8_t original[4];

    if (_proc->ReadMemory(_address, (char *)original, 4)) return false;

    _originalOpcode.assign(original, original + 4);

    return _proc->WriteMemory(_address, (char *)opcode, sizeof(opcode), true);
}

bool ARMv7SoftwareBreakpoint::Reset() {
    return _proc->WriteMemory(_address, (char *)_originalOpcode.data(),
                              _originalOpcode.size(), true);
}

bool THUMBv7SoftwareBreakpoint::Apply() {
    // may need to reverse endian
    static uint8_t opcode[] = {0x00, 0xBE};

    uint8_t original[2];

    if (_proc->ReadMemory(_address, (char *)original, 2)) return false;

    _originalOpcode.assign(original, original + 2);

    return _proc->WriteMemory(_address, (char *)opcode, sizeof(opcode), true);
}

bool THUMBv7SoftwareBreakpoint::Reset() {
    return ARMv7SoftwareBreakpoint::Reset();
}
