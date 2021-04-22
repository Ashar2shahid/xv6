# XV6

## Nice Implementation 

To implement nice we need to first assign a priority value to each created process to do so, first initialize priority in the PCB in proc.h

```
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  int priority;                // process priority <--------------------------
};
```

Now when a new process is allocated we need to set the priority so inside proc.c allocproc function we set priority of new process 

```
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->priority = 4; <------------------------------

  ........
}
  ```

and inside exec.c when a parent process creates a child process we need to set the priority
```
int
exec(char *path, char **argv){
  ...........................
  ...........................
  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;
  proc->priority = 2; <-----------------------
  switchuvm(proc);
  freevm(oldpgdir);
  return 0;
}
```

to add the system call we need to first define it in syscall.c ,syscall.h and usys.S

`syscall.c`
```
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_sps(void);
extern int sys_nice(void); <----------------------------
extern int sys_chprio(void);
extern int sys_random(void);

static int (*syscalls[])(void) = {
....
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_sps]     sys_sps,
[SYS_chprio]  sys_chprio,
[SYS_random]  sys_random,
[SYS_nice]    sys_nice, <--------------------------------
};
```
`syscall.h`
```
#define SYS_sleep  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_sps    22
#define SYS_nice   23 <--------------------------
#define SYS_chprio 24
#define SYS_random 25
```
`usys.S`
```
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(sps)
SYSCALL(chprio)
SYSCALL(nice) <-------------------------
SYSCALL(random)
```

Now we need to define the logic of the system call inside sysproc.c

`sysproc.c`

```
int
sys_nice(void)
{
  int inc;

  argint(0, &inc);

  if (inc + proc->priority >= 20)
    proc->priority = 19;
  else if (inc + proc->priority <= -20)
    proc->priority = -20;
  else
    proc->priority += inc;

  return (int) proc->priority;
}
```

for the current process we accept an integer that can change the priority of a process if the sum is >= 20 we set the priorit as 19 otherwise is its less than -20 we set it as -20.

finally we can define the nice system call inside `user.h` to import it into our programs

```
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int sps(void);
int chprio(int pid, int priority);
int nice(int inc); <-------------------------
uint random(void);
```


We can run `nicetest` to verify that the nice value of a process is being set succesfully.


## Random Implementation

Random was implemented similar to nice as a system call, with the exeption of the logic being defined in random.c and then referencing random.c in defs.h and sysproc.c, everything else in the `syscall.h syscall.c usys.S ` is similar to nice.

`defs.h`
```
// random.c
uint            random(void);
int             randomrange(int, int);
```

`sysproc.c`
```
int
sys_random(void)
{
  return random();
}
```

random.c implements a xorshift implementation of PRNG 
refrenced from [here](https://stackoverflow.com/questions/1167253/implementation-of-rand)

```
uint
random(void)
{
  static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
  unsigned int b;
  b  = ((z1 << 6) ^ z1) >> 13;
  z1 = ((z1 & 4294967294U) << 18) ^ b;
  b  = ((z2 << 2) ^ z2) >> 27; 
  z2 = ((z2 & 4294967288U) << 2) ^ b;
  b  = ((z3 << 13) ^ z3) >> 21;
  z3 = ((z3 & 4294967280U) << 7) ^ b;
  b  = ((z4 << 3) ^ z4) >> 12;
  z4 = ((z4 & 4294967168U) << 13) ^ b;

  return (z1 ^ z2 ^ z3 ^ z4) / 2;
}
```

and also a range function 

```
// Return a random integer between a given range.
int
randomrange(int lo, int hi)
{
  if (hi < lo) {
    int tmp = lo;
    lo = hi;
    hi = tmp;
  }
  int range = hi - lo + 1;
  return random() % (range) + lo;
}
```

you can test the randomness of the system call by running `randomtest`


## Scheduler Implementation

The Lottery Scheduler algorithm is defined in proc.c . Each process has a specified amount of tickets based on its prioirty. The number of tickets is defined by:

```
uint
numtickets(int priority)
{
  int tickets = 1;
  int i;
  for (i = 20; i > priority; i--) {
    tickets = tickets + 1;
  }
  return tickets;
}

uint
totaltickets(void)
{
  struct proc *p;
  uint total = 0;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state != RUNNABLE)
      continue;
      int priority = p->priority;
      total += numtickets(priority);
      // cprintf("%d numtickets: %d\n", p->pid, numtickets(nice));
  }
  return total;
}
```

Each RUNNABLE process has a priority val between 0 and 20.
The number of tickets they have is 20 - nice + 1.
Processes with the lowest priority, 20, would have 20 - 20 + 1 = 1 ticket.
Processes with the highest priority, 0, would have 20 - 0 + 1, 21 tickets.
totaltickets() returns the sum of all these for all RUNNABLE processes.


The schduler code is defined below the else (You need to run with LOTTERY flag in MAKEFILE to enable lottery based scheduling, by default in the code the flag is set).

```
    struct proc *p;
    // Lottery Based Scheduling
    for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    // get total number of tickets by iterating through every process
    uint total = totaltickets();
    if (total == 0) {
      release(&ptable.lock);
      continue;
    }
    // cprintf("total: %d\n", total);

    // hold lottery
    uint counter = 0; // used to track if we've found the winner yet
    uint winner = randomrange(1, (int) total);
    // cprintf("winner: %d\n", winner);

    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      int priority = p->priority;
      counter += numtickets(priority);
      if (counter < winner)
        continue;

      // cprintf("Winner: %s, [pid %d]\n", p->name, p->pid);

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;

      switchuvm(p);
      p->state = RUNNING;

      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
      break;
    }
    release(&ptable.lock);
  }  
```

At every process scheduling interval run a lottery and find a winner based on the total number of tickets from `totaltickets`, the process that wins the lottery is allowed to run until the scheduler is called again. Only those processes that are RUNNABLE are allowed to enter the lottery otherwise we loop over them.

## Scheduler Test Cases

### Test Case 1 

In this test case we ran 4 processes with 10 tickets each and 4 processes with 15 tickets each for 2 minutes

We observed the following                  
       
        
        
![results](https://i.ibb.co/dgR6X2N/lotterytest1.png)

| Process No | Tickets | Ticks In Millions | % Tickets | % Ticks |
|:----------:|:-------:|:-----------------:|:---------:|:-------:|
|      0     |    10   |         6         |     10    |   10.1  |
|      1     |    10   |         5         |     10    |   8.5   |
|      2     |    10   |         6         |     10    |   10.1  |
|      3     |    10   |         6         |     10    |   10.1  |
|      4     |    15   |         9         |     15    |   15.2  |
|      5     |    15   |         9         |     15    |   15.2  |
|      6     |    15   |         10        |     15    |    17   |
|      7     |    15   |         8         |     15    |   13.5  |

The Percentage Ticks are similar to the % Tickets which means the scheduler is schduling correctly


### Test Case 2 
In this Test case we ran 6 processes with 3,3,3,5,10 and 15 tickets respectively for 2 minutes

We observed the following

![results](https://i.ibb.co/JFFJHgh/lotterytest2.png)

| Process No | Tickets | Ticks In Millions | % Tickets | % Ticks |
|:----------:|:-------:|:-----------------:|:---------:|:-------:|
|      1     |    3    |         6         |    7.7    |   10.1  |
|      2     |    3    |         7         |    7.7    |  11.86  |
|      3     |    3    |         5         |    7.7    |   8.5   |
|      4     |    5    |         8         |   12.82   |  13.55  |
|      5     |    10   |         15        |   25.64   |   25.4  |
|      6     |    15   |         18        |   38.46   |   30.5  |

The percentage Ticks are inline with the expected outcome, if we increase the number of processes and run it for longer periods of time it will converge to values similar to % Tickets, therefore the scheduler is schduling correctly 


## Test Case 3

In this Test case we run 6 processes with 10 tickets each, when process 6 reaches 10M ticks we reduce the number of tickets to 2. 

Process 6 hit 10M ticks at exatly 1:01 and we observed the following:

![results](https://i.ibb.co/zh9SJrC/lotterytest3-1.png)

| Process No | Tickets | Ticks In Millions | % Tickets | % Ticks |
|:----------:|:-------:|:-----------------:|:---------:|:-------:|
|      1     |    10   |         4         |   16.67   |  12.21  |
|      2     |    10   |         6         |   16.67   |  18.18  |
|      3     |    10   |         4         |   16.67   |  12.21  |
|      4     |    10   |         5         |   16.67   |  15.15  |
|      5     |    10   |         4         |   16.67   |  12.21  |
|      6     |    10   |         10        |   16.67   |   30.3  |

After 2 minutes we observed the following:
| Process No | Tickets | Ticks In Millions After 2 mins | Diff ticks (2min -1min) | % Tickets | % Ticks |
|:----------:|:-------:|:------------------------------:|-------------------------|:---------:|:-------:|
|      1     |    10   |               10               | 6                       |   19.23   |  16.21  |
|      2     |    10   |               14               | 8                       |   19.23   |   21.6  |
|      3     |    10   |               11               | 7                       |   19.23   |   18.9  |
|      4     |    10   |               12               | 7                       |   19.23   |   18.9  |
|      5     |    10   |               11               | 7                       |   19.23   |   18.9  |
|      6     |    2    |               12               | 2                       |    3.84   |   5.4   |

As we can see the number of ticks reduced to the expected probability after we reduced the number of tickets at minute 1.











