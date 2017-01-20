// This program can be freely used to test your scheduler. It is
// by no means a complete test.

#include "types.h"
#include "user.h"
#include "param.h"

// PrioCount should be set to the nummber of priority levels
#define PrioCount 7
#define numChildren 10

#ifdef CS333_P3

void
countForever(int p)
{
  int j;
  unsigned long count = 0;

  j = getpid();
  p = p%PrioCount;
  setpriority(j, p);
  printf(1, "%d: start prio %d\n", j, p);
  //printf(1, "%d running\n", j);
  //printf(1, "%d running\n", j);

  while (1) {
    count++;

    //if (count % 10000000 == 0)
      //printf(1, "%d running\n", j);

    if ((count & 0xFFFFFFF) == 0) {
      //p = (p+1) % PrioCount;
      //setpriority(j, p);
      //printf(1, "%d: new prio %d\n", j, p);
      printf(1, "%d running\n", j);
    }
  }
}

int
main(void)
{
  int i, rc;

  printf(1, "\nRunning test_sched. %d children processes, %d queues\n", numChildren, PrioCount);

  for (i=0; i<numChildren; i++) {
    rc = fork();
    if (!rc) { // child
      countForever(i);
    } else {
      //printf(1, "childProc forked. pid=%d, prio=%d\n", rc, i % PrioCount);
    }
  }
  // what the heck, let's have the parent waste time as well!
  //countForever(1);
  wait();

  exit();
}
#endif