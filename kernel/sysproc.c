#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
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


// Read pgaccess_test() in user/pgtlbtest.c to see how pgaccess is used.
// Start by implementing sys_pgaccess() in kernel/sysproc.c.
// You'll need to parse arguments using argaddr() and argint().
// For the output bitmask, it's easier to store a temporary buffer in the kernel and copy it to the user (via copyout()) after filling it with the right bits.
// It's okay to set an upper limit on the number of pages that can be scanned.
// walk() in kernel/vm.c is very useful for finding the right PTEs.
// You'll need to define PTE_A, the access bit, in kernel/riscv.h. Consult the RISC-V privileged architecture manual to determine its value.
// Be sure to clear PTE_A after checking if it is set. Otherwise, it won't be possible to determine if the page was accessed since the last time pgaccess() was called (i.e., the bit will be set forever).
// vmprint() may come in handy to debug page tables.


// #ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 sa;
  int pg_num;
  uint64 res;

  argaddr(0, &sa);
  argint(1, &pg_num);
  argaddr(2, &res);

  if (pg_num > 40){
    return 1;
  }
  struct proc* p = myproc();
  pagetable_t pt = p->pagetable;

  unsigned int abits = 0;

  for (int i=0;i<pg_num;i++){
    pte_t* pa = walk(pt, sa+i*PGSIZE, 0);
    if (*pa & (PTE_A)){
      abits = abits | (1<<i);
      *pa = *pa & (~PTE_A);
    }
  }

  copyout(pt, res, (char*)&abits, sizeof(abits));

  return 0;  
}
// #endif

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
