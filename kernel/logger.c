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
  logger.end_ticks = 0;
}

int logger_works() {
  acquire(&logger.lock);

  int result = 0;
  if (logger.end_ticks == -1 || (logger.end_ticks > 0 && ticks <= logger.end_ticks))
    result = 1;

  release(&logger.lock);

  return result;
}

uint64
sys_logger_enable(void) {
  acquire(&logger.lock);
  logger.end_ticks = -1;
  release(&logger.lock);
  return 0;
}

uint64
sys_logger_enable_ticks(void) {
  int ts;
  argint(0, &ts);

  acquire(&logger.lock);
  logger.end_ticks = ticks + ts;
  release(&logger.lock);

  return 0;
}

uint64
sys_logger_disable(void) {
  acquire(&logger.lock);
  logger.end_ticks = 0;
  release(&logger.lock);
  return 0;
}