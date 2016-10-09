#include "../src/Process.h"
#include <string.h>
int main() {
  int pid;

  printf("pid: ");
  scanf("%d", &pid);

  auto proc = Process::GetProcess(pid);
}
