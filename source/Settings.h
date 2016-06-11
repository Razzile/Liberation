//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//

#ifndef __Settings__
#define __Settings__

#include <CoreFoundation/CoreFoundation.h>

inline namespace liberation {
class Settings {
public:
    Settings(const char* path);
    ~Settings();

    int GetPrefInt(const char* key);
    float GetPrefFloat(const char* key);
    bool GetPrefBool(const char* key);

    __attribute__((noinline)) bool reloadSettings();

    class settings_proxy {
    public:
        char* key;

        union Value {
            int asInt;
            bool asBool;
            float asFloat;
        } value;

        enum ValueType { Int, Bool, Float } valueType;

        Settings* container;

        settings_proxy(const char* _key) {
            key = (char*)malloc(strlen(_key));
            strcpy(key, _key);
        }

        settings_proxy(int val) {
            value.asInt = val;
            valueType = Int;
        }

        settings_proxy(float val) {
            value.asFloat = val;
            valueType = Float;
        }

        settings_proxy(bool val) {
            value.asBool = val;
            valueType = Bool;
        }

        __attribute__((noinline)) operator int() {
            return container->GetPrefInt(key);
        }

        __attribute__((noinline)) operator float() {
            return container->GetPrefFloat(key);
        }

        __attribute__((noinline)) operator bool() {
            return container->GetPrefBool(key);
        }

        settings_proxy& operator=(const settings_proxy& source) {
            switch (source.valueType) {
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

        ~settings_proxy() {
            if (key != NULL) free(key);
        }
    };

    settings_proxy operator[](const char* key) {
        settings_proxy proxy(key);
        proxy.container = this;
        return proxy;
    }

private:
    const char* path;
    CFDictionaryRef dict;
};
}  // liberation
#endif
