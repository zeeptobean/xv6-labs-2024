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

uint64
sys_trace(void)
{
  argaddr(0, &myproc()->tracemask);
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  info.freemem = countfreemem();
  info.nproc = countfreeproc();
  info.nopenfiles = countopenfiles();

  uint64 addr;
  argaddr(0, &addr);
  if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0) return -1;
  return 0;
}

uint64
sys_pgaccess(void)
{
  uint64 base_addr;
  int n;
  uint64 output_bitmask_addr;
  uint64 bitmask;

  argaddr(0, &base_addr);
  argint(1, &n);
  argaddr(2, &output_bitmask_addr);

  if(n < 0 || n > 64) return -1;
  if(pgaccess(myproc()->pagetable, base_addr, n, &bitmask) < 0) return -1;

  if(copyout(myproc()->pagetable, output_bitmask_addr, (char *)&bitmask, sizeof(bitmask)) < 0) return -1;
  return 0;
}

uint64
sys_vmprint(void)
{
  vmprint(myproc()->pagetable);
  return 0;
}