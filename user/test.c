#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  int f = open("aaa", O_RDWR|O_CREATE);
  printf("f = %d\n", f);
  printf("%d\n", write(f, "hello", 5));
  close(f);
  exit(0);
}