#include "types.h"
#include "user.h"
#include "param.h"

#ifdef CS333_P3
void
test_set_bad_pri()
{

  int rc;
  printf(1, "test_set_bad_pri - setting bad priority value -1 now\n");
  rc = setpriority(getpid(), -1);

  if (rc != 0)
    printf(1, "[PASS] test_set_bad_pri\n");
  else
    printf(1, "[FAIL] test_set_bad_pri - unexpected return code from setpriority %d\n", rc);
}

void
test_set_bad_pid()
{

  int rc;
  printf(1, "test_set_bad_pid - setting bad pid value 12345 now\n");
  rc = setpriority(12345, 0);

  if (rc != 0)
    printf(1, "[PASS] test_set_bad_pid\n");
  else
    printf(1, "[FAIL] test_set_bad_pid - unexpected return code from setpriority %d\n", rc);
}

// this 
void
test_set_good()
{

  int rc;
  printf(1, "test_set_good - calling setpriority with valid arguments now\n");
  rc = setpriority(getpid(), 0);

  if (rc == 0)
    printf(1, "[PASS] test_set_good\n");
  else
    printf(1, "[FAIL] test_set_good - unexpected return code from setpriority %d\n", rc);
}

void
test_verify_correct()
{

  int f;
  int rc;
  printf(1, "test_verify_correct - changing the priority of this process (pid=%d) to MAX_PRIORITY\n", getpid());
  rc = setpriority(getpid(), MAX_PRIORITY);
  char *argv[1];
  if (rc == 0) {

    f = fork();
    if (f < 0) {
      printf(1, "fork error \n");
    } else if (f == 0) {
      argv[0] = "ps";
      exec("ps", argv);
    } else {
      wait();
    }
  } else {
    printf(1, "[FAIL] test_verify_correct - unexpected return code from setpriority %d\n", rc);
  }


}

int
main(int argc, char *argv[])
{
  printf(1, "\n*****  Running PKprioritytests  *****\n");

  test_set_bad_pri();
  test_set_bad_pid();
  test_set_good();
  test_verify_correct();

  printf(1, "\n*****  Completed PKprioritytests *****\n");

  exit();
}
#endif