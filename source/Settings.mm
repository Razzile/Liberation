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
    dict = (CFDictionaryRef)[NSDictionary dictionaryWithContentsOfFile:[NSString stringWithUTF8String:path]];
}

Settings::~Settings() {
    [(NSDictionary *)dict release];
}

int Settings::GetPrefInt(const char *key) {
    return [[(NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] intValue];
}

float Settings::GetPrefFloat(const char *key) {
    return [[(NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] floatValue];
}

bool Settings::GetPrefBool(const char *key) {
    return [[(NSDictionary *)dict objectForKey:[NSString stringWithUTF8String:key]] boolValue];
}
