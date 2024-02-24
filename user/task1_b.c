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

  kill(cid);

  int exit_status;
  // wait is useful, because <cid> is killed and
  // exit_status will always be -1
  // but it's ok...
  wait(&exit_status);

  printf("%d %d\n", cid, exit_status);

  exit(0);
}
