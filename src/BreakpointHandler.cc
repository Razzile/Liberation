//
//  BreakpointHandler.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "BreakpointHandler.h"

std::shared_ptr<BreakpointHandler> BreakpointHandler::SharedHandler() {
    static BreakpointHandler main;
    return std::make_shared<BreakpointHandler>(main); // std::move needed?
}

bool BreakpointHandler::InstallBreakpoint(Breakpoint* bp, bool immActive) {
    if (bp) {
        _breakpoints.push_back(BreakpointRef(bp));
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
        if (it->get() == bp) {
            bool success = (*it)->Reset();
            if (!success) return false;
            _breakpoints.erase(it);
        }
    }
    return true;
}

bool BreakpointHandler::EnableBreakpoint(Breakpoint *bp) {
    return bp->Apply();
}

bool BreakpointHandler::DisableBreakpoint(Breakpoint *bp) {
    return bp->Reset();
}

Breakpoint *BreakpointHandler::BreakpointAtAddress(vm_address_t address) {
    for (BreakpointRef& bp : _breakpoints) {
        if (bp->Address() == address) return bp.get();
    }
    return nullptr;
}

std::vector<Breakpoint *> BreakpointHandler::Breakpoints(uint32_t flags) {
    std::vector<Breakpoint *> local;
    for (BreakpointRef& bp : _breakpoints) {
        if (flags & BKPT_ENABLED) {
            if (bp->Active()) local.push_back(bp.get());
        }
        if (flags & BKPT_DISABLED) {
            if (!bp->Active()) local.push_back(bp.get());
        }
    }

    return local;
}
