#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"

extern int readCount;

int
sys_fork(void)
{
  return fork();
}

int
sys_getreadcount(void)
{
  return readCount;
}

/* The following code is added by Marcelo Mejia, mxm210189, and Caleb Scarmardo, cds210003 */

int
sys_settickets(void)
{
  int desiredTickets;

  if (argint(0, &desiredTickets) < 0)		// If argint returns < 0 (unable to retreive tickets from process), return -1
   return -1;
  if (desiredTickets < 1)			// If the process wants their tickets to be < 1 (invalid), return -1
   return -1;
  proc->ticketNum = desiredTickets;		// Update the number of tickets for the current process
  return 0;
}

/* End of code added */

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
