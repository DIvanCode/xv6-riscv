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
  // enable all loggers
  if (argc == 1) {
    for (int i = 0; i < N_TLOGS; ++i)
      logger_enable(i);
    exit(0);
  }

  // enable exact logger (argv[1])
  if (argc == 2) {
    for (int i = 0; i < N_TLOGS; ++i) {
      if (strcmp(loggers[i], argv[1]) == 0) {
        logger_enable(i);
        exit(0);
      }
    }

    fprintf(2, "logger not found: %s\n", argv[1]);
    exit(0);
  }

  // enable all loggers for atoi(argv[2]) ticks
  if (argc == 3 && strcmp(argv[1], "-t") == 0) {
    int ts = atoi(argv[2]);
    if (ts < 0) {
      fprintf(2, "incorrect ticks: %s\n", argv[2]);
      exit(0);
    }
    for (int i = 0; i < N_TLOGS; ++i)
      logger_enable_ticks(i, ts);
    exit(0);
  }

  // enable exact logger for atoi(argv[3]) ticks
  if (argc == 4 && strcmp(argv[2], "-t") == 0) {
    int ts = atoi(argv[3]);
    if (ts < 0) {
      fprintf(2, "incorrect ticks: %s\n", argv[2]);
      exit(0);
    }
    for (int i = 0; i < N_TLOGS; ++i) {
      if (strcmp(loggers[i], argv[1]) == 0) {
        logger_enable_ticks(i, ts);
        exit(0);
      }
    }

    fprintf(2, "logger not found: %s\n", argv[1]);
    exit(0);
  }

  fprintf(2, "error: unknown format\n");
  exit(0);
}