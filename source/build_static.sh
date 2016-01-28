#!/bin/bash
SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
xcrun --sdk iphoneos /Users/callum/mods-n-stuff/Coding/o-llvm-build/bin/clang++ -arch armv7 -arch arm64 -isystem/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk --sysroot=$SYSROOT -I/opt/theos/include/ -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1  -std=c++11 -stdlib=libc++ -lc++ -L/opt/theos/lib/ -lsubstrate -mios-version-min=5.0 -mllvm -sub -mllvm -fla -O3 Patch.cpp -c
xcrun --sdk iphoneos clang++ -arch armv7 -arch arm64 -isystem/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk --sysroot=$SYSROOT -I/opt/theos/include/ -std=c++11 -stdlib=libc++ -lc++ -L/opt/theos/lib/ -lsubstrate -mios-version-min=5.0 -O3 Settings.mm -c
libtool Patch.o Settings.o -o civ3.a
