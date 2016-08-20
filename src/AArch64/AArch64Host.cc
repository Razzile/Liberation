//
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "AArch64Host.h"
#include <sys/sysctl.h>

int AArch64Host::HardwareBreakpointCount() {
  // for some stupid reason, ARMv8 prevents the debug information register
  // (ID_AA64DFR0_EL1) from being accessed in userland, so we have to use sysctl

  int count = 0;
  size_t len = sizeof(count);
  if (sysctlbyname("hw.optional.breakpoint", &count, &len, 0, 0) == 0) {
    return count;
  }
  return 0;
}

int AArch64Host::HardwareWatchpointCount() {
  // for some stupid reason, ARMv8 prevents the debug information register
  // (ID_AA64DFR0_EL1) from being accessed in userland, so we have to use sysctl

  int count = 0;
  size_t len = sizeof(count);
  if (sysctlbyname("hw.optional.watchpoint", &count, &len, 0, 0) == 0) {
    return count;
  }
  return 0;
}

Host *Host::CurrentHost() {
  static Host *host = nullptr;
  if (!host)
    host = new AArch64Host();
  return host;
}
