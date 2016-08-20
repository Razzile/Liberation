//
//  Breakpoints.h
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#ifndef _Breakpoints_
#define _Breakpoints_

#include <functional>
#include <mach/mach.h>
#include <vector>

class Process;
class Breakpoint;
class ThreadState;

using byte = uint8_t;
using BreakpointCallback = std::function<void(Breakpoint *, ThreadState *)>;

class Breakpoint {
public:
  Breakpoint(Process *proc, vm_address_t address)
      : _address(address), _proc(proc) {}
  virtual ~Breakpoint() {}

  virtual bool Apply() = 0;
  virtual bool Reset() = 0;

  virtual bool Active() { return this->_active; }
  virtual vm_address_t Address() { return this->_address; }
  virtual BreakpointCallback Callback() { return this->_callback; }

  virtual void AddCallback(BreakpointCallback cb) { _callback = cb; }

protected:
  bool _active;
  vm_address_t _address;
  Process *_proc;
  BreakpointCallback _callback;
};

class SoftwareBreakpoint : public Breakpoint {
public:
  SoftwareBreakpoint(Process *proc, vm_address_t address)
      : Breakpoint(proc, address) {}
  ~SoftwareBreakpoint() = default;

  virtual bool Apply();
  virtual bool Reset();

protected:
  std::vector<byte> _originalBytes;
};

class HardwareBreakpoint : public Breakpoint {
public:
  HardwareBreakpoint(Process *proc, vm_address_t address)
      : Breakpoint(proc, address) {}
  ~HardwareBreakpoint() = default;

  virtual bool Apply();
  virtual bool Reset();
};

using BreakpointRef = std::shared_ptr<Breakpoint>;

#endif /* _Breakpoints_ */
