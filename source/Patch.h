//--------------------------------//
//--------Code-Inject-v3----------//
//-------Created-by-Razzile-------//
//--------------------------------//
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <mach/mach.h>
#include <substrate.h>

#define hidden __attribute__((visibility("hidden")))

inline namespace utils {
class Patch {
public:
	~Patch();
	Patch(unsigned long long addr, uint data);
	Patch(unsigned long long addr, std::string data);


	bool Apply();
	bool Reset();

private:
	unsigned long long patchAddr;
	unsigned char *patchData;
	unsigned char *origData;
	size_t patchSize;
};

// make virtual?

class Hook {
public:
	~Hook();
	Hook(std::string symbol, void *hookPtr, void **origPtr);
	Hook(std::string symbol, void *hookPtr);
	Hook(void *hookFuncAddr, void *hookPtr, void **origPtr);
	Hook(void *hookFuncAddr, void *hookPtr);
    Hook(vm_address_t hookFuncAddr, void *hookPtr, void **origPtr);
    Hook(vm_address_t hookFuncAddr, void *hookPtr);

	template <typename T> hidden
	Hook(void *hookFuncAddress, T *hookPtr, T **origPtr) :
		Hook(hookFuncAddress, (void*)hookPtr, (void**)origPtr)
	{}

	template <typename T> hidden
	Hook(void *hookFuncAddress, T *hookPtr) :
		Hook(hookFuncAddress, (void*)hookPtr)
	{}

	template <typename T> hidden
	Hook(vm_address_t hookFuncAddr, T *hookPtr, T **origPtr) :
		Hook((void*)(hookFuncAddr), (void*)hookPtr, (void**)origPtr)
	{}

	template <typename T> hidden
	Hook(vm_address_t hookFuncAddr, T *hookPtr)  :
		Hook((void*)(hookFuncAddr), (void*)hookPtr)
	{}

	bool Apply();
	bool Reset();
private:
	std::string symbol;
	void *hookPtr;
	void **origPtr;
	void *hookFuncAddr;
	unsigned char *origData;
	size_t patchSize;
};
} // utils
