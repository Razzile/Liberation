#include <stdio.h>
#include <unistd.h>

int __main() {
  printf("%d\n", getpid());
  while (1)
    sleep(1);
}
