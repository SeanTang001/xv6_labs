#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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
  if(n < 0)
    n = 0;
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


// trace a function
uint64
sys_trace(void)
{
  int mask;

  argint(0, &mask);

  myproc()->trace_mask = mask;

  return 0;
}

// store running info
uint64
sys_sysinfo(void)
{

  struct sysinfo s;
  uint64 st; // user pointer to struct stat

  argaddr(0, &st);

  struct proc *p = myproc();

  // s->nproc = (uint64) nproc();
  // s->freemem = (uint64) kfreemem();
  // printf("nproc: %d\n", nproc());
  // printf("freemem: %d\n", kfreemem());

  s.nproc = nproc();
  s.freemem = kfreemem();

  if(copyout(p->pagetable, st, (char *)&s, sizeof(s)) < 0)
    return -1;
  return 0;
}
