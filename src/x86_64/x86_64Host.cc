//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "x86_64Host.h"

int x86_64Host::HardwareBreakpointCount() {
  return 4; // always 4
}

int x86_64Host::HardwareWatchpointCount() {
  return 4; // always 4
}
