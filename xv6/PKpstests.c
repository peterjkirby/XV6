#include "types.h"
#include "user.h"
#include "uproc.h"

static void
test_getprocs_strict_max(uint max)
{
  int used;
  struct uproc *utable;

  utable = malloc(max * sizeof(struct uproc));

  used = getprocs(max, utable);

  if (used == -1) {
    printf(1, "[FAIL] test_getprocs_strict_max_%d - getprocs returned a nonzero value (%d) with max argument %d\n", max, used, max);
  } else {
    if (max < used) {
      printf(1, "[FAIL] test_getprocs_strict_max_%d - getprocs filled more entries in the uproc table, %d,  than the maximum allowed, %d\n", max, used, max);
    } else {
      printf(1, "[PASS] test_getprocs_strict_max_%d \n", max);
    }

  }

}

static void
test_getprocs_populated_correctly()
{
  int used;
  struct uproc *utable;

  utable = malloc(10 * sizeof(struct uproc));

  used = getprocs(10, utable);

  if (used == -1) {
    printf(1, "[FAIL] test_getprocs_populated_correctly - getprocs returns -1. \n");
  } else {

    int fail = 0;
    for (int i = 0; i < used && !fail; ++i) {
      if (utable[i].pid == 0) {
        fail = 1;
      }
    }

    if (fail) {
        printf(1, "[FAIL] test_getprocs_populated_correctly - a process that was indicated to have been populated was not. used=%d \n", used);
    } else {
        printf(1, "[PASS] test_getprocs_populated_correctly \n");
    }

  }
}


int
main(int argc, char *argv[])
{
  printf(1, "\n*****  Running PKpstests  *****\n");

  test_getprocs_strict_max(1);
  test_getprocs_strict_max(16);
  test_getprocs_strict_max(64);
  test_getprocs_strict_max(72);

  test_getprocs_populated_correctly();

  printf(1, "\n*****  Completed PKpstests *****\n");

  exit();
}