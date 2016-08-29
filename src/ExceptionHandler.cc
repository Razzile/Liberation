//
//  ExceptionHandler.cc
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "ExceptionHandler.h"
#include "BreakpointHandler.h"
#include "Host.h"
#include "mach_exc.h"
#include "x86_64/x86_64ThreadState.h"
#include <mach/mach.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

extern "C" boolean_t mach_exc_server(mach_msg_header_t *, mach_msg_header_t *);

#pragma mark Ugly
// aren't mach exceptions so nice
extern "C" kern_return_t
catch_mach_exception_raise(mach_port_t exception_port, mach_port_t thread,
                           mach_port_t task, exception_type_t type,
                           exception_data_t code,
                           mach_msg_type_number_t code_count) {
  Exception exception(exception_port, thread, task, type, code, code_count);

  auto handler = ExceptionHandler::SharedHandler();
  return handler->ExceptionCallback(exception);
}

extern "C" kern_return_t catch_mach_exception_raise_state(
    mach_port_t exception_port, exception_type_t exception,
    exception_data_t code, mach_msg_type_number_t code_count, int *flavor,
    thread_state_t in_state, mach_msg_type_number_t in_state_count,
    thread_state_t out_state, mach_msg_type_number_t *out_state_count) {
  return KERN_FAILURE;
}

extern "C" kern_return_t catch_mach_exception_raise_state_identity(
    mach_port_t exception_port, mach_port_t thread, mach_port_t task,
    exception_type_t exception, exception_data_t code,
    mach_msg_type_number_t code_count, int *flavor, thread_state_t in_state,
    mach_msg_type_number_t in_state_count, thread_state_t out_state,
    mach_msg_type_number_t *out_state_count) {
  return KERN_FAILURE;
}

void *server_thread(void *arg) {
  mach_port_t exc_port = *(mach_port_t *)arg;
  kern_return_t kr;

  while (1) {
    if ((kr = mach_msg_server_once(mach_exc_server, 4096, exc_port, 0)) !=
        KERN_SUCCESS) {
      fprintf(stderr, "mach_msg_server_once: error %s\n",
              mach_error_string(kr));
    }
  }
  return NULL;
}

ThreadState *Exception::ThreadState() {
  Host *host = Host::CurrentHost();
  switch (host->Platform()) {
  //   case Platform::AArch64: return new AArch64ThreadState()
  case Platform::x86_64: {
    x86_64ThreadState *state = new x86_64ThreadState(_thread);
    state->Load();
    return state;
  }

  default:
    return nullptr;
  }
}

std::shared_ptr<ExceptionHandler> ExceptionHandler::SharedHandler() {
  static std::shared_ptr<ExceptionHandler> instance =
      std::make_shared<ExceptionHandler>();
  return instance;
}

bool ExceptionHandler::SetupHandler() {
  kern_return_t kr = 0;
  mach_port_t exc_port;
  mach_port_t task = mach_task_self();
  pthread_t exception_thread;
  int err;

  mach_msg_type_number_t maskCount = 1;
  exception_mask_t mask;
  exception_handler_t handler;
  exception_behavior_t behavior;
  thread_state_flavor_t flavor;

  if ((kr = mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE, &exc_port)) !=
      KERN_SUCCESS) {
    fprintf(stderr, "mach_port_allocate: %s\n", mach_error_string(kr));
    return false;
  }

  if ((kr = mach_port_insert_right(task, exc_port, exc_port,
                                   MACH_MSG_TYPE_MAKE_SEND)) != KERN_SUCCESS) {
    fprintf(stderr, "mach_port_allocate: %s\n", mach_error_string(kr));
    return false;
  }

  if ((kr = task_get_exception_ports(task, EXC_MASK_ALL, &mask, &maskCount,
                                     &handler, &behavior, &flavor)) !=
      KERN_SUCCESS) {
    fprintf(stderr, "task_get_exception_ports: %s\n", mach_error_string(kr));
    return false;
  }

  if ((err = pthread_create(&exception_thread, NULL, server_thread,
                            &exc_port)) != 0) {
    fprintf(stderr, "pthread_create server_thread: %s\n", strerror(err));
    return false;
  }

  pthread_detach(exception_thread);

  if ((kr = task_set_exception_ports(task, EXC_MASK_BREAKPOINT, exc_port,
                                     EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
                                     flavor)) != KERN_SUCCESS) {
    fprintf(stderr, "task_set_exception_ports: %s\n", mach_error_string(kr));
    return false;
  }
  return true;
}

kern_return_t ExceptionHandler::ExceptionCallback(Exception &exception) {
  thread_suspend(exception._thread);
  _exceptionHistory.push_back(exception);

  auto bkptHandler = BreakpointHandler::SharedHandler();

  int type = exception._type;
  // currently only handle breakpoints, pass all other exceptions to parent
  // handler
  if (type != EXC_BREAKPOINT)
    return KERN_FAILURE; // possibly MIG_DESTROY_REQUEST

  ThreadState *state = exception.ThreadState();
  if (state) {
    Breakpoint *bkpt =
        bkptHandler->BreakpointAtAddress(state->CurrentAddress());
    printf("breakpoint obj at %p [active: %d]\n", bkpt, bkpt->active());
    if (bkpt && bkpt->active()) {
      printf("inner reached\n");

      auto cb = bkpt->callback();
      if (cb) {
        cb(*state);
      }
      state->Save();
      bkptHandler->DisableBreakpoint(bkpt);
      thread_resume(exception._thread);
      return KERN_SUCCESS;
    }
  }
  thread_resume(exception._thread);
  return KERN_FAILURE; // possibly MIG_DESTROY_REQUEST
}
