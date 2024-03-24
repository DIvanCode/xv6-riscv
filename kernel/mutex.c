#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "stat.h"
#include "proc.h"

// global mutex table
struct {
  struct spinlock lock;
  struct mutex mutex[NMUTEX];
} mtable;

// init global mutex table at the start
void
mutexinit(void) {
  initlock(&mtable.lock, "mtable");

  acquire(&mtable.lock);
  for (int i = 0; i < NMUTEX; ++i)
    initsleeplock(&mtable.mutex[i].lock, "mtable_i");
  release(&mtable.lock);
}

struct mutex*
mutexalloc(void) {
  struct mutex *m;

  acquire(&mtable.lock);
  for (m = mtable.mutex; m < mtable.mutex + NMUTEX; ++m) {
    if (m->ref == 0) {
      m->ref = 1;
      m->pid = -1;
      release(&mtable.lock);
      return m;
    }
  }
  release(&mtable.lock);

  return 0;
}

int
mutexlock(struct mutex *m) {
  acquiresleep(&m->lock);
  m->pid = myproc()->pid;
  return 0;
}

struct mutex*
mutexdup(struct mutex *m) {
  if (m->ref < 1) {
    panic("mutexdup");
  }
  m->ref++;
  return m;
}

int
mutexunlock(struct mutex *m) {
  if (m->pid != myproc()->pid)
    return -1;

  m->pid = -1;
  releasesleep(&m->lock);
  return 0;
}

int
mutexclose(struct mutex *m) {
  if (m->ref < 1)
    return -1;

  --m->ref;
  if (m->pid == myproc()->pid)
    m->pid = -1;
  return 0;
}

// alloc mutex descriptor in proc's mutex table
static int
mdalloc(struct mutex *m)
{
  int md;
  struct proc *p = myproc();

  for (md = 0; md < NOMUTEX; md++) {
    if (p->omutex[md] == 0) {
      p->omutex[md] = m;
      return md;
    }
  }
  return -1;
}

// read mutex descriptor from args and takes mutex from proc's mutex table
static int
argmd(int n, int *pmd, struct mutex **pm) {
  int md;
  struct mutex *m;

  argint(n, &md);
  if (md < 0 || md >= NOMUTEX || (m = myproc()->omutex[md]) == 0)
    return -1;

  if (pmd)
    *pmd = md;
  if (pm)
    *pm = m;
  return 0;
}

uint64
sys_mopen(void) {
  struct mutex *m;
  int md;

  if ((m = mutexalloc()) < 0 || (md = mdalloc(m)) < 0)
    return -1;

  return md;
}

uint64
sys_lock(void) {
  struct mutex *m;

  if (argmd(0, 0, &m) < 0)
    return -1;

  return mutexlock(m);
}

uint64
sys_unlock(void) {
  struct mutex *m;

  if (argmd(0, 0, &m) < 0)
    return -1;

  return mutexunlock(m);
}

uint64
sys_mclose(void) {
  int md;
  struct mutex *m;

  if (argmd(0, &md, &m) < 0)
    return -1;

  myproc()->omutex[md] = 0;
  return mutexclose(m);
}
