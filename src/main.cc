//
//  main.cpp
//  Liberation
//
//  Copyright © 2016 Satori. All rights reserved.
//

#include "BreakpointHandler.h"
#include "ExceptionHandler.h"
#include "Process.h"
#include "x86_64/x86_64Breakpoint.h"
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>

__attribute__((noinline)) __attribute__((naked)) int test(int x) {
  asm("nop");
  asm("mov %rdi, %rax");
  asm("ret");
}

int main(int argc, char **argv) {
  auto excHandler = ExceptionHandler::SharedHandler();
  excHandler->SetupHandler();
  std::thread([]() {
    auto bkptHandler = BreakpointHandler::SharedHandler();

    x86_64HardwareBreakpoint *bkpt =
        new x86_64HardwareBreakpoint(Process::Self().get(), (vm_address_t)test);

    bkpt->AddCallback([](ThreadState &state) {
      // woo
      state["RDI"] = 255;
    });

    bool res = bkptHandler->InstallBreakpoint(bkpt);
    printf("install was %s\n", (res) ? "successful" : "unsuccsessful");
  }).join();

  int x = test(10); // arg1 will be set to 255
  printf("test returned: %d\n", x);
}

// int main_old() {
//   mach_msg_type_number_t count;
//   thread_act_port_array_t threads;
//   unsigned number;
//   kern_return_t ret;
//   x86_thread_state64_t state;

//  mach_port_t mythread = pthread_mach_thread_np(pthread_self());

// Get a list of all the threads in the current task

//   ret = task_threads(mach_task_self(), &threads, &number);
//
//   //  thread_suspend(threads[0]);
//
//   thread_get_state(threads[0], x86_THREAD_STATE64, (thread_state_t)&state,
//                    &count);
//
//   x86_64ThreadState threadState((thread_state_t)&state);
//   threadState.Load();
//   uint64_t rax = threadState["RAX"];
//   double rax_float = threadState["RAX"];
//
//   std::cout << threadState.Description() << std::hex << rax << std::endl;
// }

// void test1();
// void test2();
//
// const char *msg = "This is how you reginject";
//
// extern "C" boolean_t mach_exc_server(mach_msg_header_t *, mach_msg_header_t
// *);
//
//// uint8_t tramp[] = { 0x48, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/// 0xFF,
//// 0xE0 };
// uint8_t tramp[] = {0xFF, 0xE0};
// void trampoline();
//
// bool in_use;
// x86_thread_state64_t saved_state;
//
// const char *registers[] = {"RAX", "RBX", "RCX", "RDX",    "RDI", "RSI",
// "RBP",
//                           "RSP", "R8",  "R9",  "R10",    "R11", "R12", "R13",
//                           "R14", "R15", "RIP", "RFLAGS", "CS",  "FS",  "GS"};
//
// void print_state(const x86_thread_state64_t &state) {
//  for (int i = 0; i < sizeof(state)/sizeof(uint64_t); i++) {
//    printf("%s: %lld [0x%llx]\n", registers[i],
//           *(uint64_t *)(uint64_t(&state) + 8*i),
//           *(uint64_t *)(uint64_t(&state) + 8*i));
//  }
//}
//
//// void *create_trampoline(uint8_t *code, size_t len) {
////    void *tramp = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
////    MAP_ANON | MAP_JIT | MAP_PRIVATE, -1, 0);
////    memcpy(tramp, code, len);
////    return tramp;
////}
//
// extern "C" kern_return_t catch_mach_exception_raise(
//    mach_port_t exception_port, mach_port_t thread, mach_port_t task,
//    exception_type_t type, exception_data_t code,
//    mach_msg_type_number_t code_count) {
//  x86_thread_state64_t state;
//  kern_return_t kr;
//  mach_msg_type_number_t count = x86_THREAD_STATE64_COUNT;
//
//  thread_suspend(thread);
//
//  if ((kr = thread_get_state(thread, x86_THREAD_STATE64,
//  (thread_state_t)&state,
//                             &count)) != KERN_SUCCESS) {
//    mach_error("thread_get_state", kr);
//  }
//  printf("suspended thread\n");
//
//  print_state(state);
//  // debug_state.__bvr[0] = uint32_t(test_func) & ~0x1;
//  // debug_state.__bcr[0] = (0 << 21) | (0xF << 5) |
//  // (2 << 1) | 0;
//
//  if (!in_use && type == EXC_BREAKPOINT) {
//    saved_state = state;
//    void *addr = (void *)test2;
//    //    state.__rdi = (uint64_t)test2;
//    state.__rip = (uint64_t)addr;
//    in_use = true;
//  } else {
//    state = saved_state;
//    state.__rip += 1;
//    in_use = false;
//  }
//
//  thread_set_state(thread, x86_THREAD_STATE64, (thread_state_t)&state,
//                   x86_THREAD_STATE64_COUNT);
//  thread_resume(thread);
//  printf("resumed thread\n");
//  return KERN_SUCCESS;
//}
//
// extern "C" kern_return_t catch_mach_exception_raise_state(
//    mach_port_t exception_port, exception_type_t exception,
//    exception_data_t code, mach_msg_type_number_t code_count, int *flavor,
//    thread_state_t in_state, mach_msg_type_number_t in_state_count,
//    thread_state_t out_state, mach_msg_type_number_t *out_state_count) {
//  exit(1);
//  return KERN_FAILURE;
//}
//
// extern "C" kern_return_t catch_mach_exception_raise_state_identity(
//    mach_port_t exception_port, mach_port_t thread, mach_port_t task,
//    exception_type_t exception, exception_data_t code,
//    mach_msg_type_number_t code_count, int *flavor, thread_state_t in_state,
//    mach_msg_type_number_t in_state_count, thread_state_t out_state,
//    mach_msg_type_number_t *out_state_count) {
//  exit(1);
//  return KERN_FAILURE;
//}
//
// void *server_thread(void *arg) {
//  mach_port_t exc_port = *(mach_port_t *)arg;
//  kern_return_t kr;
//
//  while (1) {
//    if ((kr = mach_msg_server_once(mach_exc_server, 4096, exc_port, 0)) !=
//        KERN_SUCCESS) {
//      fprintf(stderr, "mach_msg_server_once: error %#x\n", kr);
//      exit(1);
//    }
//  }
//  return NULL;
//}
//
// void register_handler() {
//  kern_return_t kr = 0;
//  mach_port_t exc_port;
//  mach_port_t task = mach_task_self();
//  pthread_t exception_thread;
//  int err;
//
//  mach_msg_type_number_t maskCount = 1;
//  exception_mask_t mask;
//  exception_handler_t handler;
//  exception_behavior_t behavior;
//  thread_state_flavor_t flavor;
//
//  if ((kr = mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE, &exc_port)) !=
//      KERN_SUCCESS) {
//    fprintf(stderr, "mach_port_allocate: %#x\n", kr);
//    exit(1);
//  }
//
//  if ((kr = mach_port_insert_right(task, exc_port, exc_port,
//                                   MACH_MSG_TYPE_MAKE_SEND)) != KERN_SUCCESS)
//                                   {
//    fprintf(stderr, "mach_port_allocate: %#x\n", kr);
//    exit(1);
//  }
//
//  if ((kr = task_get_exception_ports(task, EXC_MASK_ALL, &mask, &maskCount,
//                                     &handler, &behavior, &flavor)) !=
//      KERN_SUCCESS) {
//    fprintf(stderr, "task_get_exception_ports: %#x\n", kr);
//    exit(1);
//  }
//
//  if ((err = pthread_create(&exception_thread, NULL, server_thread,
//                            &exc_port)) != 0) {
//    fprintf(stderr, "pthread_create server_thread: %s\n", strerror(err));
//    exit(1);
//  }
//
//  pthread_detach(exception_thread);
//
//  if ((kr = task_set_exception_ports(task, EXC_MASK_BREAKPOINT, exc_port,
//                                     EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
//                                     flavor)) != KERN_SUCCESS) {
//    fprintf(stderr, "task_set_exception_ports: %#x\n", kr);
//    exit(1);
//  }
//}
//
//__attribute__((naked)) void trampoline() {
//  asm("push %rbp; mov %esp, %ebp; call *%rdi; pop %rbp");
//}
//
//__attribute__((naked)) void test1() { asm("int $3"); }
//
// void test2() { printf("test 2\n"); }
