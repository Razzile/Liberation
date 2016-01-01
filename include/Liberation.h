//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//
//------------Licenses------------//
//--------------------------------//
// Copyright (C) 2016  Razzile
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#if __cplusplus <= 199711L
  #error Please enable C++11 for use with Liberation
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <mach/mach.h>
#include <CoreFoundation/CoreFoundation.h>

#define hidden __attribute__((visibility("hidden")))

/* Container namespace for classes */
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
	class Settings {
	public:
	    Settings(const char *path);
	    ~Settings();

	    int GetPrefInt(const char* key);
	    float GetPrefFloat(const char* key);
	    bool GetPrefBool(const char* key);

	    __attribute__((noinline))
	    bool reloadSettings();

	    class settings_proxy {
	    public:
	        char* key;

	        union Value {
	            int asInt;
	            bool asBool;
	            float asFloat;
	        } value;

	        enum ValueType {
	            Int,
	            Bool,
	            Float
	        } valueType;

	        Settings *container;

	        hidden settings_proxy(const char* _key) {
	            key = (char*)malloc(strlen(_key));
	            strcpy(key, _key);
	        }

	        hidden settings_proxy(int val) {
	            value.asInt = val;
	            valueType = Int;
	        }

	        hidden settings_proxy(float val) {
	            value.asFloat = val;
	            valueType = Float;
	        }

	        hidden settings_proxy(bool val) {
	            value.asBool = val;
	            valueType = Bool;
	        }

	        hidden
	        operator int() {
	            return container->GetPrefInt(key);
	        }

	        hidden
	        operator float() {
	            return container->GetPrefFloat(key);
	        }

	        hidden
	        operator bool() {
	            return container->GetPrefBool(key);
	        }

	        hidden settings_proxy& operator= (const settings_proxy &source) {
	            switch(source.valueType)
	            {
	                case Int: {
	                    set(source.value.asInt);
	                    break;
	                }
	                case Bool: {
	                    set(source.value.asBool);
	                    break;
	                }
	                case Float: {
	                    set(source.value.asFloat);
	                    break;
	                }
	            }
	            return *this;
	        }
	        void set(bool value);
	        void set(int value);
	        void set(float value);

	        hidden ~settings_proxy() {
	            if (key != NULL)
	                free(key);
	        }
	    };

	    hidden  settings_proxy operator[] (const char* key) {
	        settings_proxy proxy(key);
	        proxy.container = this;
	        return proxy;
	    }
	private:
	    const char *path;
	    CFDictionaryRef dict;
	};
} // utils
