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


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif

#ifdef LAB_PGTBL
uint64
sys_pgaccess(void) {
    uint64 start_addr;
    int num_pages;
    uint64 user_bitmask;
    uint64 bitmask = 0;


    argaddr(0, &start_addr);
    argint(1, &num_pages);
    argaddr(2, &user_bitmask);


    if (num_pages > 64) {
        return -1;
    }

    for (int i = 0; i < num_pages; i++) {
        pte_t *pte;
        uint64 va = start_addr + i * PGSIZE;

        pte = walk(myproc()->pagetable, va, 0);
        if (pte == 0 || (*pte & PTE_V) == 0) {
            printf("Invalid page\n");
            continue; 
        }

        // Check the PTE_A bit
        if (*pte & PTE_A) {
            bitmask |= (1L << i); // Set the corresponding bit in the bitmask
            *pte &= ~PTE_A;      // Clear the PTE_A bit
        }
    }

    // Copy the bitmask to user space
    if (copyout(myproc()->pagetable, user_bitmask, (char *)&bitmask, sizeof(bitmask)) < 0) {
        return -1;
    }

    return 0;
}
#endif


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
