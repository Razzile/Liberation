//
//  x86_64ThreadState.h
//  ghetto_hook2
//
//  Copyright © 2016 Satori. All rights reserved.
//

#ifndef _x86_64ThreadState_
#define _x86_64ThreadState_

#include "ThreadState.h"

class x86_64ThreadState : public ThreadState {
public:
  x86_64ThreadState(thread_state_t state) : ThreadState(state) {}

  virtual std::string Description() override;
  virtual Register &operator[](std::string key) override;
  virtual bool Load() override;
  virtual vm_address_t CurrentAddress() override;

private:
};

#endif /* _x86_64ThreadState_ */
