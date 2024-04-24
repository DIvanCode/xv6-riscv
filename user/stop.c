#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/log_types.h"
#include "user/user.h"

const char *loggers[N_TLOGS] = {
  [TLOG_EXEC] = "exec",
  [TLOG_SWITCH] = "switch",
  [TLOG_SYSCALL] = "syscall",
  [TLOG_DEVINTR] = "devintr"
};

int
main(int argc, char *argv[])
{
  // stop all loggers
  if (argc == 1) {
    for (int i = 0; i < N_TLOGS; ++i)
      logger_disable(i);
    exit(0);
  }

  // stop exact logger
  if (argc == 2) {
    for (int i = 0; i < N_TLOGS; ++i) {
      if (strcmp(loggers[i], argv[1]) == 0) {
        logger_disable(i);
        exit(0);
      }
    }

    fprintf(2, "logger not found: %s\n", argv[1]);
    exit(0);
  }

  fprintf(2, "error: unknown format\n");
  exit(0);
}