//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//

#include <Foundation/Foundation.h>
#include <string>
#include "Settings.h"


Settings::Settings(const char *path) : path(path) {
    std::string temp = path;
    if (temp.find("/") == std::string::npos) {
        temp = std::string("/var/mobile/Library/Preferences/") + path;
    }
    if (temp.find(".plist") == std::string::npos) {
        temp = temp + ".plist";
    }
    path = temp.data();
    dict = (__bridge CFDictionaryRef)[NSDictionary dictionaryWithContentsOfFile:[NSString stringWithUTF8String:path]];
}

Settings::~Settings() {
    CFRelease(dict);
}

int Settings::GetPrefInt(const char *key) {
    return [[(__bridge NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] intValue];
}

float Settings::GetPrefFloat(const char *key) {
    return [[(__bridge NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] floatValue];
}

bool Settings::GetPrefBool(const char *key) {
    return [[(__bridge NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] boolValue];
}
