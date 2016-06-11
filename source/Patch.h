//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//

#include <mach/mach.h>
#include <sys/types.h>
#include <sstream>
#include <string>
#include <vector>

#define hidden __attribute__((visibility("hidden")))

inline namespace liberation {
using bytes = std::vector<uint8_t>;

enum class ARMv7Mode { ARM, Thumb };

class Patch {
public:
    static Patch *CreatePatch(vm_address_t address, uint32_t data);
    static Patch *CreatePatch(vm_address_t address, std::string data);
    static Patch *CreateRawPatch(vm_address_t addr, char *data, size_t len);
    static Patch *CreateInstrPatch(vm_address_t address, std::string instr,
                                   ARMv7Mode mode = ARMv7Mode::Thumb);

    virtual bool Apply();
    virtual bool Reset();

private:
    Patch() = default;
    Patch(vm_address_t addr, char *data, size_t len);
    ~Patch();

protected:
    vm_address_t _address;
    bytes _patchBytes;
    bytes _origBytes;
    size_t _patchSize;
};

class Hook {
public:
    ~Hook();
    Hook(std::string symbol, void *hookPtr, void **origPtr);
    Hook(std::string symbol, void *hookPtr);
    Hook(void *hookFuncAddr, void *hookPtr, void **origPtr);
    Hook(void *hookFuncAddr, void *hookPtr);
    Hook(vm_address_t hookFuncAddr, void *hookPtr, void **origPtr);
    Hook(vm_address_t hookFuncAddr, void *hookPtr);

    template <typename T>
    hidden Hook(void *hookFuncAddress, T *hookPtr, T **origPtr)
    : Hook(hookFuncAddress, (void *)hookPtr, (void **)origPtr) {}

    template <typename T>
    hidden Hook(void *hookFuncAddress, T *hookPtr)
    : Hook(hookFuncAddress, (void *)hookPtr) {}

    template <typename T>
    hidden Hook(vm_address_t hookFuncAddr, T *hookPtr, T **origPtr)
    : Hook((void *)(hookFuncAddr), (void *)hookPtr, (void **)origPtr) {}

    template <typename T>
    hidden Hook(vm_address_t hookFuncAddr, T *hookPtr)
    : Hook((void *)(hookFuncAddr), (void *)hookPtr) {}

    bool Apply();
    bool Reset();

private:
    std::string _symbol;
    void *_hookPtr;
    void **_origPtr;
    void *_hookFuncAddr;
};

}  // liberation
