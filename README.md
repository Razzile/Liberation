Liberation
========

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
* (Optional) Add ```TARGET_STRIP_FLAGS = -u -r -s /dev/null``` to the top of your makefile to strip your dylib (Recommended as Liberation will otherwise contain a lot of info about your tweak)
* Add ```YOURPROJECTHERE_LIBRARIES = Liberation``` to your Makefile

That's it!

## Example

```Logos
#include <Liberation.h>

void Init() {
    Settings settings = "myPrefs.plist";

    Patch goldPatch = Patch(0x12345, 0x0000A0E1);
    Patch healthPatch = Patch(0x12346, "1EFF2FE1");
    Hook xpHook = Hook("__symbol", someFunc, &someOtherFunc);

    bool gold = settings["kGold"];
    if (gold) {
        goldPatch.Apply(); // applies patch
    }
    else {
        goldPatch.Reset(); // resets patch
    }
    int aVal = settings["kVal"]; // accessing an int from the plist
    float floatVal = settings["kFloat"]; // accessing a float from the plist
    xpHook.Apply(); // applies hook
    xpHook.Reset(); // resets hook
}
```
