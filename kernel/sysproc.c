#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_pgaccess(void) {
  uint64 page_addr;
  int npages;
  uint64 addr;

  argaddr(0, &page_addr);
  argint(1, &npages);
  argaddr(2, &addr);

  pagetable_t pagetable = myproc()->pagetable;

  int batches = (npages + 7) / 8;
  char res[batches];
  for (int i = 0; i < batches; ++i)
    res[i] = 0;
  int batch = batches - 1, pos = 0;

  for (int i = 0; i < npages; ++i) {
    pte_t *pte = walk(pagetable, page_addr, 0);
    if (*pte & PTE_A) {
      (*pte) &= ~PTE_A;
      res[batch] |= (1 << pos);
    }

    page_addr += PGSIZE;

    pos++;
    if (pos == 8) {
      batch--;
      pos = 0;
    }
  }

  if (copyout(pagetable, addr, res, sizeof(res)) < 0)
    return -1;
  return 0;
}