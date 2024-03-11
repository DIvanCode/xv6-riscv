#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid = getpid(), cid = fork();
  if (cid < 0) {
    printf("fork failed (response = %d)\n", cid);
    exit(1);
  }

  if (cid == 0) {
    sleep(100);
    exit(1);
  } else {
    printf("%d %d\n", pid, cid);
  }

  int response = kill(cid);
  if (response < 0) {
    printf("kill failed (response = %d)\n", response);
    exit(1);
  }

  int exit_status;
  response = wait(&exit_status);
  if (response < 0) {
    printf("wait failed (response = %d)\n", response);
    exit(1);
  }

  printf("%d %d\n", cid, exit_status);

  exit(0);
}
