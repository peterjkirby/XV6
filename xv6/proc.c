#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "uproc.h"

// @version 1.1
#define TICKS_TO_SEC(X,Y) (((X)-(Y)) / (100))
#define TICKS_TO_PARTIAL_SEC(X,Y) (((X)-(Y))%100)

// @version 1.2
#define DEFAULT_UID 1
#define DEFAULT_GID 1

// @version 1.3
#define NUM_READY_LIST 1
#define DEFAULT_PROC_PRI 0
#define TICKS_TO_PROMOTE 100


struct {
  struct spinlock lock;
  struct proc proc[NPROC];
  struct proc *pReadyList[NUM_READY_LIST];
  struct proc *pFreeList;
  uint promoteAtTime;
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

static void rl_enqueue(int qid, struct proc*);
static struct proc* rl_dequeue(int qid);

static void fl_push(struct proc*);
static struct proc* fl_pop();
static void priority_promote();

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

#ifndef CS333_P3
  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;
#endif

#ifdef CS333_P3
  acquire(&ptable.lock);
  p = fl_pop();
  if (p)
    goto found;
  release(&ptable.lock);
  return 0;
#endif

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  release(&ptable.lock);

  // Allocate kernel stack.
  if ((p->kstack = kalloc()) == 0) {
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof * p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof * p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof * p->context);
  p->context->eip = (uint)forkret;

  // @version 1.2
  p->cpu_ticks_in = 0;
  p->cpu_ticks_total = 0;

  // @version 1.1
  acquire(&tickslock);
  p->start_ticks = ticks;
  release(&tickslock);

  // @version 1.3
  p->next = 0;
  p->priority = DEFAULT_PROC_PRI;
  return p;
}

static void
rl_enqueue(int qid, struct proc* p)
{
  //cprintf("rl_enqueue %s\n", p->name);
  //p->next = ptable.pReadyList[qid];
  //ptable.pReadyList[qid] = p;
  
  struct proc *n;
  n = ptable.pReadyList[qid];
  if (!n) {
    ptable.pReadyList[qid] = p;
  } else {
    while (n->next)
      n = n->next;

    n->next = p;
  }

}

static struct proc*
rl_dequeue(int qid)
{
  struct proc *p;
  p = ptable.pReadyList[qid];

  if (p) {
    ptable.pReadyList[qid] = p->next;
    p->next = 0;
  }



  return p;
}

static void
fl_push(struct proc *p)
{

  p->next = ptable.pFreeList;
  ptable.pFreeList = p;

}

static struct proc*
fl_pop()
{
  struct proc *p;
  p = ptable.pFreeList;
  // TODO check for error here (when pFreeList is empty)
  ptable.pFreeList = p->next;

  return p;
}

// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  for (p = ptable.proc; p < &ptable.proc[NPROC]; ++p)
    fl_push(p);

  int rlIdx;
  for (rlIdx = 0; rlIdx < NUM_READY_LIST; ++rlIdx)
    ptable.pReadyList[rlIdx] = 0;

  ptable.promoteAtTime = 0;
  p = allocproc();
  initproc = p;
  if ((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // @version 1.2
  p->uid = DEFAULT_UID;
  p->gid = DEFAULT_GID;
  p->parent = p; // parent of first process is itself

  p->state = RUNNABLE;
  ptable.pReadyList[0] = p;
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;

  sz = proc->sz;
  if (n > 0) {
    if ((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if (n < 0) {
    if ((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  proc->sz = sz;
  switchuvm(proc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;

  // Allocate process.
  if ((np = allocproc()) == 0)
    return -1;

  // Copy process state from p.
  if ((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0) {
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = proc->sz;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for (i = 0; i < NOFILE; i++)
    if (proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  safestrcpy(np->name, proc->name, sizeof(proc->name));

  // @version 1.2
  np->uid = proc->uid;
  np->gid = proc->gid;

  pid = np->pid;

  cprintf("forked: %s \n", np->name);
  // lock to force the compiler to emit the np->state write last.
  acquire(&ptable.lock);
  np->state = RUNNABLE;


  rl_enqueue(0, np);

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *p;
  int fd;

  if (proc == initproc)
    panic("init exiting");

  // Close all open files.
  for (fd = 0; fd < NOFILE; fd++) {
    if (proc->ofile[fd]) {
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(proc->cwd);
  end_op();
  proc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(proc->parent);

  // Pass abandoned children to init.
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if (p->parent == proc) {
      p->parent = initproc;
      if (p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  proc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for (;;) {
    // Scan through table looking for zombie children.
    havekids = 0;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->parent != proc)
        continue;
      havekids = 1;
      if (p->state == ZOMBIE) {
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if (!havekids || proc->killed) {
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(proc, &ptable.lock);  //DOC: wait-sleep
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
#ifndef CS333_P3
// original xv6 scheduler. Use if CS333_P3 NOT defined.
void
scheduler(void)
{
  struct proc *p;
  int cur_ticks;

  for (;;) {
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;

      acquire(&tickslock);
      cur_ticks = ticks;
      release(&tickslock);
      p->cpu_ticks_in = cur_ticks;

      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
    release(&ptable.lock);

  }
}

#else
// CS333_P3 MLFQ scheduler implementation goes here
void
scheduler(void)
{

  struct proc *p;
  int cur_ticks;

  for (;;) {
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    
	if (cpu->id == 0) {
		// @version 1.3
		// TODO can I just grab the value of ticks here once, rather than here
		// and below? If mutli-cpu, value may change between here and below. Also,
		// can this scheduler be interrupted? ASK MM
		acquire(&tickslock);
		cur_ticks = ticks;
		release(&tickslock);
	
		if (ptable.promoteAtTime <= cur_ticks)
			priority_promote();
	}

	
	


    p = rl_dequeue(0);
    if (p) {

      // TODO remove in prod
      if (p->state != RUNNABLE)
        panic("Invalid p state in scheduler\n");

	  
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;
      
      acquire(&tickslock);
      cur_ticks = ticks;
      release(&tickslock);
      p->cpu_ticks_in = cur_ticks;

      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      
      //cprintf("after run, proc->state is %d\n", p->state);
      if (p->state == RUNNABLE)
        rl_enqueue(0, p);
      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
   

    release(&ptable.lock);

  }
}
#endif

// reprioritizes all of the processes in ptable
// assumes that a lock on ptable is already held and 
// will not be released for the duration of this method
static void
priority_promote() 
{
	int cur_ticks;
	int i;
	struct proc *p;

	for(i = 0; i < NUM_READY_LIST; ++i) {
		p = ptable.pReadyList[i];		
		
		while (p) {
			if (0 < p->priority)
				--p->priority;
			p = p->next;
		}
	}

	// schedule the next reprioritization
	acquire(&tickslock);
	cur_ticks = ticks;
	release(&tickslock);

	ptable.promoteAtTime = cur_ticks + TICKS_TO_PROMOTE;
	
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state.
void
sched(void)
{
  int intena;
  int cur_ticks;

  if (!holding(&ptable.lock))
    panic("sched ptable.lock");
  if (cpu->ncli != 1)
    panic("sched locks");
  if (proc->state == RUNNING)
    panic("sched running");
  if (readeflags()&FL_IF)
    panic("sched interruptible");
  intena = cpu->intena;

  acquire(&tickslock);
  cur_ticks = ticks;
  release(&tickslock);

  proc->cpu_ticks_total += (cur_ticks - proc->cpu_ticks_in);
  swtch(&proc->context, cpu->scheduler);
  cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  proc->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  if (proc == 0)
    panic("sleep");

  if (lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if (lk != &ptable.lock) { //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }

  // Go to sleep.
  proc->chan = chan;
  proc->state = SLEEPING;
  sched();

  // Tidy up.
  proc->chan = 0;

  // Reacquire original lock.
  if (lk != &ptable.lock) { //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == SLEEPING && p->chan == chan) {
      p->state = RUNNABLE;
      rl_enqueue(0, p);
    }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if (p->pid == pid) {
      p->killed = 1;
      // Wake process from sleep if necessary.
      if (p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

static void
print_float (int secs, int partial_secs)
{
  char buf1[20];
  char buf2[20];
  char buf3[40];
  char *s1;
  char *s2;

  //int len;

  // convert secs to a ascii chars starting at buf[0]
  s1 = itoa(secs, buf1, 20);

  s2 = itoa(partial_secs, buf2, 20);

  if (partial_secs < 10) {
    concat(s1, ".0", buf3);
  } else {
    concat(s1, ".", buf3);
  }

  //concat(s1, ".", buf3);
  concat(buf3, s2, buf3);

  cprintf("%-15%s ", buf3);

}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
    [UNUSED]    "unused",
    [EMBRYO]    "embryo",
    [SLEEPING]  "sleep ",
    [RUNNABLE]  "runnable",
    [RUNNING]   "run   ",
    [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  int cur_ticks;
  acquire(&tickslock);
  cur_ticks = ticks;
  release(&tickslock);


  cprintf("\n%-5%s %-5%s %-5%s %-5%s %-10%s %-20%s %-15%s %-15%s %s \n", "PID", "PPID", "UID", "GID", "State", "Name", "Elapsed (s)", "CPU Time (s)", "PCs");
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if (p->state == UNUSED)
      continue;
    if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";

    cprintf("%-5%d %-5%d %-5%d %-5%d %-10%s %-20%s ",
            p->pid,
            p->parent->pid,
            p->uid,
            p->gid,
            state,
            p->name);

    print_float(TICKS_TO_SEC(cur_ticks, p->start_ticks),
                TICKS_TO_PARTIAL_SEC(cur_ticks, p->start_ticks));

    print_float(p->cpu_ticks_total / 100, p->cpu_ticks_total % 100);

    if (p->state == SLEEPING) {
      getcallerpcs((uint*)p->context->ebp + 2, pc);
      for (i = 0; i < 10 && pc[i] != 0; i++)
        cprintf("%p", pc[i]);
    }
    cprintf("\n");
  }
}

// sets the uid of the current process
// @param uid an integer value such that 0 <= uid <= 32767
// @return 0 if successful
int
setuid(uint uid)
{
  if (uid < 0 || 32767 < uid)
    return -1;

  proc->uid = uid;
  return 0;
}

// sets the gid of the current process
// @param gid an integer value such that 0 <= gid <= 32767
// @return 0 if successful
int
setgid(uint gid)
{
  if (gid < 0 || 32767 < gid)
    return -1;

  proc->gid = gid;
  return 0;
}

int
getprocs(uint max, struct uproc *utable)
{

  static char *states[] = {
    [UNUSED]    "unused",
    [EMBRYO]    "embryo",
    [SLEEPING]  "sleep ",
    [RUNNABLE]  "runnable",
    [RUNNING]   "run   ",
    [ZOMBIE]    "zombie"
  };
  struct proc *p;
  struct uproc *up;
  int i;
  int used;
  int cur_ticks;

  up = utable;
  i = 0;

  // if utable is a null pointer, return -1 to indiciate error
  if (!utable)
    return -1;

  acquire(&tickslock);
  cur_ticks = ticks;
  release(&tickslock);

  // acquire a lock on the ptable
  acquire(&ptable.lock);

  // iterate through all othe processes in the table
  for (p = ptable.proc; p < &ptable.proc[NPROC] && i < max; p++) {
    // only fetch processes that are active (RUNNABLE, SLEEPING, or RUNNING)
    if (p->state == UNUSED || p->state == EMBRYO || p->state == ZOMBIE)
      continue;

    up->gid = p->gid;
    up->pid = p->pid;
    up->uid = p->uid;
    up->priority = p->priority;
	up->ppid = p->parent->pid;
    up->elapsed_ticks = cur_ticks - p->start_ticks;
    up->CPU_total_ticks = p->cpu_ticks_total;
    up->size = p->sz;

    safestrcpy(up->state, states[p->state], STRMAX);
    safestrcpy(up->name, p->name, STRMAX);

    ++i;
    ++up;
  }
  release(&ptable.lock);

  // record the number of entries used in the uproc table
  used = i;
  // for each unused entity in the uproc table, set the state to UNUSED
  for (; i < max; ++i) {
    safestrcpy(up->state, states[UNUSED], STRMAX);
    ++up;
  }


  return used;
}

