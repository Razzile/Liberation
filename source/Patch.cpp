//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//
#include "Patch.h"
#include <mach-o/dyld.h>
#include <mach/mach.h>
#include <substrate.h>
#include <sys/types.h>
#include "keystone/keystone.h"

#define always_inline __attribute__((always_inline)) inline

#ifdef __LP64__
#define mach_header_xx mach_header_64
#define segment_command_xx segment_command_64
#else
#define mach_header_xx mach_header
#define segment_command_xx segment_command
#endif

/*
 * sizeof(word) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */
typedef int word; /* "word" used for optimal copy speed */

#define wsize sizeof(word)
#define wmask (wsize - 1)

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */

always_inline void *liberation_memcpy(void *dst0, const void *src0,
                                      size_t length) {
    char *dst = (char *)dst0;
    const char *src = (const char *)src0;
    size_t t;

    if (length == 0 || dst == src) /* nothing to do */
        goto done;

/*
 * Macros: loop-t-times; and loop-t-times, t>0
 */
#define TLOOP(s) \
    if (t) TLOOP1(s)
#define TLOOP1(s) \
    do {          \
        s;        \
    } while (--t)

    if ((unsigned long)dst < (unsigned long)src) {
        /*
         * Copy forward.
         */
        t = (uintptr_t)src; /* only need low bits */
        if ((t | (uintptr_t)dst) & wmask) {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (uintptr_t)dst) & wmask || length < wsize)
                t = length;
            else
                t = wsize - (t & wmask);
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    } else {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (uintptr_t)src;
        if ((t | (uintptr_t)dst) & wmask) {
            if ((t ^ (uintptr_t)dst) & wmask || length <= wsize)
                t = length;
            else
                t &= wmask;
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }
done:
    return (dst0);
}

inline namespace utils {
static always_inline kern_return_t ReadAddress(vm_address_t address,
                                               unsigned char *outData,
                                               size_t size) {
    address &= ~1;
    mach_port_t port = mach_task_self();
    vm_size_t sz;
    return vm_read_overwrite(port, address, size, (vm_address_t)outData, &sz);
}

static always_inline kern_return_t WriteAddress(vm_address_t address,
                                                unsigned char *inData,
                                                size_t size) {
    kern_return_t status;
    mach_port_t port = mach_task_self();

    // sanitize address
    address &= ~1;

    vm_address_t text_base = address;
    /* set memory protections to allow us writing code there */
    status = vm_protect(port, text_base, size, false,
                        VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);

    /* check if the protection fails */
    if (status != KERN_SUCCESS) return status;

    /* write code to memory, should always be our own process for now */
    if (port == mach_task_self()) {
        liberation_memcpy((void *)address, (void *)inData, size);
        status = KERN_SUCCESS;
    } else {
        status = vm_write(port, address, (vm_address_t)inData, size);
    }
    if (status != KERN_SUCCESS) return status;

    return vm_protect(port, text_base, size, false,
                      VM_PROT_READ | VM_PROT_EXECUTE);
}
}

#pragma mark Patch

Patch *Patch::CreatePatch(vm_address_t address, uint32_t data) {
    size_t size = 0;
    if (!(((data & 0xFFFF8000) + 0x8000) & 0xFFFF7FFF)) {
        size = sizeof(unsigned short);
        data = OSSwapInt16(data);
    } else {
        data = OSSwapInt32(data);
        size = sizeof(int);
    }
    return new Patch(address, (char *)&data, size);
}

Patch *Patch::CreatePatch(vm_address_t address, std::string data) {
    Patch *patch = nullptr;

    std::string::iterator end_pos = std::remove(data.begin(), data.end(), ' ');
    data.erase(end_pos, data.end());

    const char *tempHex = data.c_str();
    size_t patchSize = data.length() / 2;
    uint8_t *patchData = new uint8_t[patchSize];

    /* convert string to hex array */
    int n;
    for (int i = 0; i < patchSize; i++) {
        sscanf(tempHex + 2 * i, "%2X", &n);
        patchData[i] = (unsigned char)n;
    }
    patch = new Patch(address, (char *)patchData, patchSize);
    delete[] patchData;

    return patch;
}

Patch *Patch::CreateRawPatch(vm_address_t addr, char *data, size_t len) {
    return new Patch(addr, data, len);
}

Patch *Patch::CreateInstrPatch(vm_address_t address, std::string instr,
                               ARMv7Mode mode) {
    Patch *patch = nullptr;
    ks_engine *ks;
    ks_err err;
    size_t count;
    unsigned char *encode;
    size_t size;

#ifdef __LP64__
    ks_arch arch = KS_ARCH_ARM64;
#else
    ks_arch arch = KS_ARCH_ARM;
    if (mode == ARMv7Mode::ARM)
        err = ks_open(arch, KS_MODE_ARM, &ks);
    else
        err = ks_open(arch, KS_MODE_THUMB, &ks);
#endif

    if (err != KS_ERR_OK) {
        return nullptr;
    }

    if (ks_asm(ks, instr.data(), address, &encode, &size, &count) !=
        KS_ERR_OK) {
        return nullptr;
    } else {
        patch = new Patch(address, (char *)encode, size);
        free(encode);
    }
    return patch;
}

Patch::Patch(vm_address_t addr, char *data, size_t len) : _address(addr) {
    uint8_t *orig = new uint8_t[len];
    ReadAddress(addr, orig, len);

    this->_patchBytes.assign(data, data + len);
    this->_origBytes.assign(orig, orig + len);

    delete[] orig;
}

bool Patch::Apply() {
    return WriteAddress(_address, _patchBytes.data(), _patchSize);
}

bool Patch::Reset() {
    return WriteAddress(_address, _origBytes.data(), _patchSize);
}

#pragma mark Hook

Hook::Hook(std::string symbol, void *hookPtr, void **origPtr)
: _symbol(symbol), _hookPtr(hookPtr), _origPtr(origPtr) {
    this->_hookFuncAddr = MSFindSymbol(NULL, symbol.c_str());
}

Hook::Hook(std::string symbol, void *hookPtr) : Hook(symbol, hookPtr, NULL) {}

Hook::Hook(void *funcPtr, void *hookPtr, void **origPtr)
: _symbol(""), _hookPtr(hookPtr), _origPtr(origPtr), _hookFuncAddr(funcPtr) {}

Hook::Hook(void *funcPtr, void *hookPtr) : Hook(funcPtr, hookPtr, 0) {}

Hook::Hook(vm_address_t funcPtr, void *hookPtr, void **origPtr)
: Hook((void *)funcPtr, hookPtr, origPtr) {}

Hook::Hook(vm_address_t funcPtr, void *hookPtr)
: Hook((void *)funcPtr, hookPtr) {}

Hook::~Hook() {}

bool Hook::Apply() {
    MSHookFunction(_hookFuncAddr, _hookPtr, _origPtr);
    return true;
}

bool Hook::Reset() {
    return false;
}
