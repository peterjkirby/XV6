#include "types.h"
#include "user.h"
#include "uproc.h"

static void
test_etime(int sec)
{
  uint prev_t;
  uint cur_t;
  uint elapsed_t;
  int cid;
  char *argv[1];
  
  elapsed_t = 0;
    
  cid = fork();
  if (cid == 0) {
    printf(1, "[RUNNING] test_etime_%d - running for %d seconds.\n", sec, sec);
    cur_t = uptime();

    while (1) {
      prev_t = cur_t;
      cur_t = uptime();
      elapsed_t += (cur_t - prev_t);
    
      if (sec <= (elapsed_t / 100))
        break;      
    }
    argv[0] = "ps";
    exec("ps", argv);
    exit();
  } else {
    wait();
    printf(1, "[VERIFY] test_etime_%d - verify that the process with pid=%d has a CPU time >= %d\n", sec, cid, sec);
  }
  
}


int
main(int argc, char *argv[])
{
  printf(1, "\n*****  Running PKetimetests  *****\n");

  test_etime(1);
  test_etime(2);
  test_etime(3);
  
  printf(1, "\n*****  Completed PKetimetests *****\n");

  exit();
}