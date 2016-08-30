//
//  BreakpointHandler.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "BreakpointHandler.h"

std::shared_ptr<BreakpointHandler> BreakpointHandler::SharedHandler() {
  static std::shared_ptr<BreakpointHandler> instance =
      std::make_shared<BreakpointHandler>();
  return instance;
}

bool BreakpointHandler::InstallBreakpoint(Breakpoint *bp, bool immActive) {
  if (bp) {
    _breakpoints.push_back(bp);
    if (immActive) {
      bool success = bp->Apply();
      if (!success) {
        _breakpoints.pop_back();
        return false;
      }
    }
  }
  return true;
}

bool BreakpointHandler::UninstallBreakpoint(Breakpoint *bp) {
  for (auto it = _breakpoints.begin(); it != _breakpoints.end(); ++it) {
    if (*it == bp) {
      bool success = (*it)->Reset();
      if (!success)
        return false;
      _breakpoints.erase(it);
    }
  }
  return false;
}

bool BreakpointHandler::EnableBreakpoint(Breakpoint *bp) { return bp->Apply(); }

bool BreakpointHandler::DisableBreakpoint(Breakpoint *bp) {
  return bp->Reset();
}

Breakpoint *BreakpointHandler::BreakpointAtAddress(vm_address_t address) {
  for (Breakpoint *bp : _breakpoints) {
    if (bp->address() == address)
      return bp;
  }
  return nullptr;
}

std::vector<Breakpoint *> BreakpointHandler::Breakpoints(uint32_t flags) {
  std::vector<Breakpoint *> local;
  for (Breakpoint *bp : _breakpoints) {
    if (flags & BKPT_ENABLED) {
      if (bp->active())
        local.push_back(bp);
    }
    if (flags & BKPT_DISABLED) {
      if (!bp->active())
        local.push_back(bp);
    }
  }
  return local;
}
