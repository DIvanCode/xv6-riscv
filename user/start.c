#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    logger_enable();
  } else if (argc == 2) {
    int ts = atoi(argv[1]);
    logger_enable_ticks(ts);
  } else {
    printf("start: error\n");
    exit(1);
  }
  exit(0);
}