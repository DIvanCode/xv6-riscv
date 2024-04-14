#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "queue.h"

void queue_write(struct queue *queue, char x) {
  queue->data[queue->tail++] = x;
  if (queue->tail == PGSIZE * QNPAGES) {
    queue->over = 1;
    queue->tail = 0;
  }
}
