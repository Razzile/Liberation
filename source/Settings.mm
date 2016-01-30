
//--------------------------------//
//-----------Liberation-----------//
//-------Created-by-Razzile-------//
//--------------------------------//
//------Don't mess with this------//
//------Unless you are smart------//
//--------------------------------//
//------------Licenses------------//
//--------------------------------//
//Copyright (c) 2016, Razzile

//Permission to use, copy, modify, and/or distribute this software for any purpose
//with or without fee is hereby granted, provided that the above copyright notice
//and this permission notice appear in all copies.

//THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
//REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
//FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
//INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
//OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
//TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
//THIS SOFTWARE.

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
