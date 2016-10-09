#include "../src/Process.h"
#include <string.h>
int ___main() {
  int pid;

  printf("pid: ");
  scanf("%d", &pid);

  auto proc = Process::GetProcess(pid);
}
