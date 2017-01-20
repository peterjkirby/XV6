// This program can be freely used to test your scheduler. It is
// by no means a complete test.

#include "types.h"
#include "user.h"

// PrioCount should be set to the nummber of priority levels
#define PrioCount 3
#define numChildren 1

void
countForever(int p)
{
  int j;
  unsigned long count = 0;

  j = getpid();
  //p = p%PrioCount;
  //setpriority(j, p);
  //printf(1, "%d: start prio %d\n", j, p);
  //printf(1, "%d running\n", j);
  printf(1, "%d running\n", j);
  
  while (1) {
    count++;
    if ((count & 0xFFFFFFF) == 0) {
      p = (p+1) % PrioCount;
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
    }
  }
  // what the heck, let's have the parent waste time as well!
  //countForever(1);
  wait();

  exit();
}