#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SSS 1024

int
main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "go") == 0) {
    // test buffer overflow (!with QNPAGES=1 in param.h!)
    for (int i = 0; i < 100; ++i) {
      int pid = fork();
      if (pid == 0) {
        exec("vm", argv);
        exit(0);
      } else {
        wait(&pid);
      }
    }
  }

  char mesg[SSS];
  if (dmesg(mesg, SSS) < 0) {
    printf("error: dmesg\n");
    exit(0);
  }
  printf("%s", mesg);
  exit(0);
}
