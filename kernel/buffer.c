#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "queue.h"

struct queue buffer;

void
bufferinit(void) {
  initlock(&buffer.lock, "queue_lock");
  buffer.head = 0;
  buffer.tail = 0;
}

static char pr_digits[] = "0123456789abcdef";

static void
pr_msg_int(int xx, int base, int sign) {
  char buf[16];
  int i;
  uint x;

  if (sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = pr_digits[x % base];
  } while((x /= base) != 0);

  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    queue_write(&buffer, buf[i]);
}

static void
pr_msg_ptr(uint64 x) {
  int i;
  queue_write(&buffer, '0');
  queue_write(&buffer, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    queue_write(&buffer, pr_digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void
pr_msg(char *fmt, ...) {
  va_list ap;
  int i, c;
  char *s;

  queue_write(&buffer, '[');

  acquire(&tickslock);
  pr_msg_int(ticks, 10, 1);
  release(&tickslock);

  queue_write(&buffer, ']');
  queue_write(&buffer, ' ');

  if (fmt == 0)
    panic("pr_msg: null fmt");

  va_start(ap, fmt);
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      queue_write(&buffer, c);
      continue;
    }

    c = fmt[++i] & 0xff;
    if (c == 0)
      break;

    switch(c) {
      case 'd':
        pr_msg_int(va_arg(ap, int), 10, 1);
        break;
      case 'x':
        pr_msg_int(va_arg(ap, int), 16, 1);
        break;
      case 'p':
        pr_msg_ptr(va_arg(ap, uint64));
        break;
      case 's':
        if ((s = va_arg(ap, char*)) == 0)
        s = "(null)";
        for (; *s; s++)
          queue_write(&buffer, *s);
        break;
      case '%':
        queue_write(&buffer, '%');
        break;
      default:
        queue_write(&buffer, '%');
        queue_write(&buffer, c);
        break;
    }
  }
  va_end(ap);

  queue_write(&buffer, '\n');
}

uint64
sys_dmesg(void) {
  char zero = 0;

  uint64 addr;
  argaddr(0, &addr);

  uint64 n;
  argaddr(1, &n);

  if (addr == 0)
    return -1;

  acquire(&buffer.lock);

  int i = 0, h = buffer.head;
  while (i + 1 < n && h != buffer.tail) {
    if (copyout(myproc()->pagetable, addr + (i++), &buffer.data[h], 1) < 0) {
      release(&buffer.lock);
      return -1;
    }

    h++;
    if (h == PGSIZE * QNPAGES)
      h = 0;
  }

  if (copyout(myproc()->pagetable, addr + i, &zero, 1) < 0) {
    release(&buffer.lock);
    return -1;
  }

  buffer.head = h;

  release(&buffer.lock);
  return 0;
}
