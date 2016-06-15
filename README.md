Liberation
========
Please note liberation is due for a project reformat, meaning all code is subject to change and Including a *proper* build script. Ignore the mess for now.

Liberation (formerly civ3) is a next generation code injection library for iOS cheaters everywhere.
Unlike liberation's predecessor writeData, which was pretty much never updated and had tons of random edits floating around the net, Liberation will be updated regularly in this repo, and contributions are welcome from anyone (even if you aren't a member of ioscheaters)

## Installation

To install  Liberation, please follow these steps:

* Download Liberation from the [Releases](https://github.com/iOSCheaters/Liberation/releases) page.
* Copy the lib folder contents to $THEOS/lib
* Copy the include folder to $THEOS/include
* (Optional) copy the nic template folder to $THEOS/templates/ios/liberation

## Usage

To use Liberation, follow these steps:

* Inlcude <Liberation.h> in your Tweak.xm (or any other (obj)c++ file)
* Use liberation functions in your tweak
* Add ```YOURPROJECTHERE_CFLAGS += -std=c++11``` to your makefile to enable c++11 (required by Liberation)
* (Optional) Add ```TARGET_STRIP_FLAGS = -u -r -s /dev/null``` to the top of your makefile to strip your dylib (Recommended as Liberation will otherwise contain a lot of info about your tweak). Must build using ```make DEBUG=0```
* Add ```YOURPROJECTHERE_LIBRARIES = Liberation c++``` to your Makefile

That's it!

## Example

```Logos
#include <Liberation.h>

void Init() {
    Settings settings = "myPrefs.plist";

    // create patch from ARM hex
    Patch *goldPatch = Patch::CreatePatch(0x12345, 0x0000A0E1);
    // create patch from instruction string
    Patch *healthPatch = Patch::CreateInstrPatch(0x12346, "MOV RO, R7; BX LR");

    // create a hook patch
    Hook xpHook = Hook("__symbol", someFunc, &someOtherFunc);

    bool gold = settings["kGold"];
    if (gold) {
        goldPatch->Apply(); // applies patch
    }
    else {
        goldPatch->Reset(); // resets patch
    }
    int aVal = settings["kVal"]; // accessing an int from the plist
    float floatVal = settings["kFloat"]; // accessing a float from the plist
    xpHook.Apply(); // applies hook
    xpHook.Reset(); // resets hook
}
```

## License

```Logos
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
```
