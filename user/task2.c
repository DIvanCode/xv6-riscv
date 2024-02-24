#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fds[2];
  if(pipe(fds) < 0){
    printf("pipe failed\n");
    exit(1);
  }
  
  for (int i = 1; i < argc; ++i) {
    int len = strlen(argv[i]);
    if (write(fds[1], argv[i], len) != len) {
      close(fds[0]);
      close(fds[1]);
      printf("write failed\n");
      exit(1);
    }
    if (i + 1 < argc) {
      if (write(fds[1], " ", 1) != 1) {
        close(fds[0]);
        close(fds[1]);
        printf("write failed\n");
        exit(1);
      }
    }
  }
  if (write(fds[1], "\n", 1) != 1) {
    close(fds[0]);
    close(fds[1]);
    printf("write failed\n");
    exit(1);
  }
  close(fds[1]);

  int cid = fork();
  if (cid == 0) {
    char c;
    do {
      if (read(fds[0], &c, 1) != 1) {
        close(fds[0]);
        printf("read failed\n");
        exit(1);
      }
      printf("%c", c);
    } while (c != '\n');
    close(fds[0]);

    exit(0);
  }

  wait((int*) 0);

  exit(0);
}
