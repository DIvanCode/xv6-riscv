#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"
#include "logger.h"

struct logger logger;

void logger_init() {
  initlock(&logger.lock, "logger");
  for (int i = 0; i < N_TLOGS; ++i)
    logger.end_ticks[i] = 0;
}

int logger_works(int who) {
  acquire(&logger.lock);

  int result = 0;
  if (logger.end_ticks[who] == -1 ||
      (logger.end_ticks[who] > 0 && ticks <= logger.end_ticks[who]))
    result = 1;

  release(&logger.lock);

  return result;
}

uint64
sys_logger_enable(void) {
  int who;
  argint(0, &who);

  if (who < 0 || who >= N_TLOGS)
    return -1;

  acquire(&logger.lock);
  logger.end_ticks[who] = -1;
  release(&logger.lock);

  return 0;
}

uint64
sys_logger_enable_ticks(void) {
  int who;
  argint(0, &who);

  if (who < 0 || who >= N_TLOGS)
    return -1;

  int ts;
  argint(1, &ts);

  if (ts < 0)
    return -1;

  acquire(&logger.lock);
  logger.end_ticks[who] = ticks + ts;
  release(&logger.lock);

  return 0;
}

uint64
sys_logger_disable(void) {
  int who;
  argint(0, &who);

  if (who < 0 || who >= N_TLOGS)
    return -1;

  acquire(&logger.lock);
  logger.end_ticks[who] = 0;
  release(&logger.lock);
  
  return 0;
}