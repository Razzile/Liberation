//
//  BreakpointHandler.h
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#ifndef _BreakpointHandler_
#define _BreakpointHandler_

#include "Breakpoint.h"
#include "Process.h"

#define BKPT_ENABLED (1 << 0)
#define BKPT_DISABLED (1 << 1)
#define BKPT_ALL (BKPT_ENABLED | BKPT_DISABLED)

using byte = uint8_t;

class BreakpointHandler {
public:
  BreakpointHandler() = default;
  static std::shared_ptr<BreakpointHandler> SharedHandler();

  bool InstallBreakpoint(Breakpoint *bp, bool immActive = true);
  bool UninstallBreakpoint(Breakpoint *bp);
  bool DisableBreakpoint(Breakpoint *bp);
  bool EnableBreakpoint(Breakpoint *bp);

  Breakpoint *BreakpointAtAddress(vm_address_t address);

  std::vector<Breakpoint *> Breakpoints(uint32_t flags = BKPT_ALL);

private:
  Process *proc;
  std::vector<Breakpoint *> _breakpoints; // TODO: switch to smart pointer?
};

#endif /* _BreakpointHandler_ */
