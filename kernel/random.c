#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

static uint64 uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

static uint64 rotr64(uint64 x, unsigned int r) {
    return (x >> r) | (x << ((-r) & 63));
}

//Return 0 failure, otherwise always return [1, 2^64-1] as rng result
uint64
sys_random(void)
{
    uint64 addr;
    uint64 seed;
    struct proc *p = myproc();
    argaddr(0, &addr);
    int addr_is_invalid = 0; 

    if(addr == 0 || addr >= p->sz) {
        uint64 oldsz = p->sz;
        if(growproc(8) < 0) return 0;
        addr = oldsz;
        seed = uptime();
        addr_is_invalid = 1;
    } else {
        if(copyin(p->pagetable, (char *)&seed, addr, sizeof(seed)) < 0) {
            return 0;
        }
    }
    
    static const uint64 multiplier = 6364136223846793005ULL;
    static const uint64 increment  = 1442695040888963407ULL;
    uint64 state = seed;
    seed = state * multiplier + increment;
    uint64 xorshifted = ((state >> 18) ^ state) >> 27;
    uint32 rot = rotr64(xorshifted, 59);
    uint64 out = (state ^ rot) * multiplier;
    out ^= (out >> 32);
    if (out == 0) return ~0ULL;

    if(addr_is_invalid == 0) {
        if(copyout(p->pagetable, addr, (char *)&seed, sizeof(seed)) < 0) return 0;
    }
    return out;
}