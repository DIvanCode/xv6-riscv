#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid = getpid(), cid = fork();

  if (cid == 0) {
    sleep(100);
    exit(1);
  } else {
    printf("%d %d\n", pid, cid);
  }

  int exit_status;
  wait(&exit_status);

  printf("%d\n", exit_status);

  exit(0);
}
