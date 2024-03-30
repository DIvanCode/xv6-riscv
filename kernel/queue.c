#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "queue.h"

void queue_write(struct queue *queue, char x) {
  acquire(&queue->lock);

  queue->data[queue->tail++] = x;
  if (queue->tail == PGSIZE * QNPAGES)
    queue->tail = 0;

  release(&queue->lock);
}
