#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SSS 1024

int
main(int argc, char *argv[]) {
  char mesg[SSS];
  if (dmesg(mesg, SSS) < 0) {
    printf("error: dmesg\n");
    exit(0);
  }
  printf("%s", mesg);
  exit(0);
}
