//--------------------------------//
//--------Code-Inject-v3----------//
//-------Created-by-Razzile-------//
//--------------------------------//

#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <sys/types.h>
#include "Patch.h"

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
typedef	int word;		/* "word" used for optimal copy speed */

#define	wsize	sizeof(word)
#define	wmask	(wsize - 1)

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */


always_inline void * liberation_memcpy(void *dst0, const void *src0, size_t length)
{
	char *dst = (char*)dst0;
	const char *src = (const char*)src0;
	size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
		goto done;

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (uintptr_t)src;	/* only need low bits */
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

inline namespace MemoryManager {
    static always_inline kern_return_t ReadAddress(vm_address_t address, unsigned char *outData, size_t size)
    {
        address &= ~1;
        mach_port_t port = mach_task_self();
        //address += _dyld_get_image_vmaddr_slide(0);
        vm_size_t sz;
        return vm_read_overwrite(port, address, size, (vm_address_t)outData, &sz);
    }

    static always_inline kern_return_t WriteAddress(vm_address_t address, unsigned char *inData, size_t size)
    {
        kern_return_t status;
        mach_port_t port = mach_task_self();

        // sanitize address
        address &= ~1;

        vm_address_t text_base = address;
        /* set memory protections to allow us writing code there */
        status = vm_protect(port, text_base, size, false, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);

        /* check if the protection fails */
        if (status != KERN_SUCCESS) exit(0); // change me

        /* write code to memory */
        if (port == mach_task_self()) {
            liberation_memcpy((void*)address, (void*)inData, size);
            status = KERN_SUCCESS;
        }
        else {
            status = vm_write(port, address, (vm_address_t)inData, size);
        }
        if (status != KERN_SUCCESS) return status;

        return vm_protect(port, text_base, size, false, VM_PROT_READ | VM_PROT_EXECUTE);
    }
}

#pragma mark Patch

Patch::Patch(unsigned long long addr, uint data) : patchAddr(addr)
{
	if (!(((data & 0xFFFF8000) + 0x8000) & 0xFFFF7FFF)) {
		this->patchSize = sizeof(unsigned short);
        data = OSSwapInt16(data);
	}
	else {
        data = OSSwapInt32(data);
		this->patchSize = sizeof(int);
	}
	this->patchData = new unsigned char[patchSize];
	this->origData = new unsigned char[patchSize];

	memcpy(patchData, &data, patchSize);
	/* read in original data */
	ReadAddress(addr, origData, patchSize);
}

Patch::Patch(unsigned long long addr, std::string data) : patchAddr(addr)
{
	/* remove all blank spaces */
	std::string::iterator end_pos = std::remove(data.begin(), data.end(), ' ');
	data.erase(end_pos, data.end());
	std::cout << data << std::endl;
	const char *tempHex = data.c_str();

	this->patchSize = data.length()/2;

	this->patchData = new unsigned char[patchSize];
	this->origData = new unsigned char[patchSize];

	/* convert string to hex array */
	int n;
	for(int i = 0; i < patchSize; i++) {
		sscanf(tempHex+2*i, "%2X", &n);
		patchData[i] = (unsigned char)n;
	}
	/* read in original data */
	ReadAddress(addr, origData, patchSize);
}


Patch::~Patch()
{
	delete patchData;
	delete origData;
}

bool Patch::Apply()
{
	return WriteAddress(patchAddr, patchData, patchSize);
}

bool Patch::Reset()
{
	return WriteAddress(patchAddr, origData, patchSize);
}

#pragma mark Hook

Hook::Hook(std::string symbol, void *hookPtr, void **origPtr)
: symbol(symbol), hookPtr(hookPtr), origPtr(origPtr)
{
	// 16 bytes is probably enough to cover MSHookFunction
	this->patchSize = 16;
	this->origData = new unsigned char[patchSize];
    this->hookFuncAddr = MSFindSymbol(NULL, symbol.c_str());

	ReadAddress((vm_address_t)hookFuncAddr, origData, patchSize);
}

Hook::Hook(std::string symbol, void *hookPtr) : Hook(symbol, hookPtr, NULL)
{

}

Hook::Hook(void *funcPtr, void *hookPtr, void **origPtr)
: symbol(""), hookPtr(hookPtr), origPtr(origPtr), hookFuncAddr(funcPtr)
{
	this->patchSize = 16;
	this->origData = new unsigned char[patchSize];

	ReadAddress((vm_address_t)hookFuncAddr, origData, patchSize);
    for (int i = 0; i < patchSize; i++)
        printf("%02x", origData[i]);
}

Hook::Hook(void *funcPtr, void *hookPtr) : Hook(funcPtr, hookPtr, 0)
{

}

Hook::Hook(vm_address_t funcPtr, void *hookPtr, void **origPtr) : Hook((void*)funcPtr, hookPtr, origPtr)
{

}

Hook::Hook(vm_address_t funcPtr, void *hookPtr) : Hook((void*)funcPtr, hookPtr)
{

}

Hook::~Hook()
{
	delete origData;
}

bool Hook::Apply()
{
	MSHookFunction(hookFuncAddr, hookPtr, origPtr);
	return true;
}

bool Hook::Reset()
{
    for (int i = 0; i < patchSize; i++)
        printf("%02x", origData[i]);
	return WriteAddress((vm_address_t)hookFuncAddr, origData, patchSize);
}
