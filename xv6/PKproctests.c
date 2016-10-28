#include "types.h"
#include "user.h"

#define MAX_UIDGID 32767

void
test_getuid()
{

  uint val;
  val = getuid();

  if (0 <= val && val <= MAX_UIDGID)
    printf(1, "[PASS] test_getuid\n");
  else
    printf(1, "[FAIL] test_getuid - an invalid uid was returned from getuid(): %d\n", val);
}

void
test_getgid()
{

  uint val;
  val = getgid();

  if (0 <= val && val <= MAX_UIDGID)
    printf(1, "[PASS] test_getgid\n");
  else
    printf(1, "[FAIL] test_getgid - an invalid gid was returned from getgid(): %d\n", val);
}

// this 
void
test_fork_getppid()
{
  uint ppid;
  uint pid;
  int p[2];

  pid = getpid();
  
  pipe(p);
  int cid = fork();
  if (cid < 0) {
    printf(1, "[FAIL] test_getppid - failed to fork, could not perfork verification step of test\n");
  } else if (cid == 0) {
    ppid = getppid();
    write(p[1], &ppid, sizeof(int));
    exit();
  } else {
    read(p[0], &ppid, sizeof(int));
    wait();

    if (pid != ppid) {
      printf(1, "[FAIL] test_getppid - The ppid of a child process did not match the parent processes pid. \n");
    } else {
      printf(1, "[PASS] test_getppid \n");
    }
    
  }

}

void
test_setuid()
{

  uint prev_uid;
  uint new_uid;

  prev_uid = getuid();

  setuid(prev_uid + 1);

  new_uid = getuid();

  if (prev_uid == new_uid)
    printf(1, "[FAIL] test_setuid - setuid did not change the process's uid \n");
  else if (new_uid != prev_uid + 1)
    printf(1, "[FAIL] test_setuid - setuid changed the process's uid to the wrong value. prev_uid=%d, new_uid=%d \n", prev_uid + 1, new_uid);
  else
    printf(1, "[PASS] test_setuid \n");
}

void
test_setgid()
{

  uint prev_gid;
  uint new_gid;

  prev_gid = getgid();

  setgid(prev_gid + 1);

  new_gid = getuid();

  if (prev_gid == new_gid)
    printf(1, "[FAIL] test_setgid - setuid did not change the process's gid \n");
  else if (new_gid != prev_gid + 1)
    printf(1, "[FAIL] test_setgid - setuid changed the process's gid to the wrong value. prev_gid=%d, new_gid=%d \n", prev_gid + 1, new_gid);
  else
    printf(1, "[PASS] test_setgid \n");
}

void
test_bad_gid_not_set()
{

  int fail;
  uint prev;
  uint curr;
  prev = getgid();
  fail = setgid(MAX_UIDGID + 1);
  curr = getgid();

  if (fail) {
    if (MAX_UIDGID < curr) {
      printf(1, "[FAIL] test_bad_gid_not_set - setgid returned nonzero, but set an invalid gid value \n");
    } else {
      printf(1, "[PASS] test_bad_gid_not_set\n");
    }
  } else {
    if (MAX_UIDGID < curr) {
      printf(1, "[FAIL] test_bad_gid_not_set - setgid returned 0 and set an invalid gid value\n");
    } else {
      if (curr == prev) {
        printf(1, "[FAIL] test_bad_gid_not_set - setgid returned 0, but did not change the gid value\n");
      } else {
        printf(1, "[FAIL] test_bad_gid_not_set - setgid returned 0, but changed the gid value to an expected value %d\n", curr);
      }
    }
  }
}

void
test_bad_uid_not_set()
{

  int fail;
  uint prev;
  uint curr;
  prev = getuid();
  fail = setuid(MAX_UIDGID + 1);
  curr = getuid();

  if (fail) {
    if (MAX_UIDGID < curr) {
      printf(1, "[FAIL] test_bad_uid_not_set - setuid returned nonzero, but set an invalid uid value \n");
    } else {
      printf(1, "[PASS] test_bad_uid_not_set\n");
    }
  } else {
    if (MAX_UIDGID < curr) {
      printf(1, "[FAIL] test_bad_uid_not_set - setuid returned 0 and set an invalid uid value\n");
    } else {
      if (curr == prev) {
        printf(1, "[FAIL] test_bad_uid_not_set - setuid returned 0, but did not change the uid value\n");
      } else {
        printf(1, "[FAIL] test_bad_uid_not_set - setuid returned 0, but changed the uid value to an expected value %d\n", curr);
      }
    }
  }
}

void
test_fork_uidgid_match_parent()
{

  uint puid;
  uint pgid;
  
  uint cuid;
  uint cgid;
  
  int cid;
  puid = getuid();
  pgid = getgid();

  int p[2];
  pipe(p);

  cid = fork();
  if (cid < 0) {
        printf(1, "[FAIL] test_fork_uidgid_match_parent - failed to fork\n");
  } else if (cid == 0) {
    cuid = getuid();
    cgid = getgid();
    write(p[1], &cuid, sizeof(uint));
    write(p[1], &cgid, sizeof(uint));
    exit();
  } else {
    read(p[0], &cuid, sizeof(uint));
    read(p[0], &cgid, sizeof(uint));  
    wait();

    if (puid != cuid || pgid != cgid) {
      printf(1, "[FAIL] test_fork_uidgid_match_parent - The child process had a different gid or uid than the parent parent process.\n");
    } else {
      printf(1, "[PASS] test_fork_uidgid_match_parent \n");
    }    
  }
}

int
main(int argc, char *argv[])
{
  printf(1, "\n*****  Running PKproctests  *****\n");

  test_getuid();
  test_getgid();
  test_setuid();
  test_setgid();
  test_bad_gid_not_set();
  test_bad_uid_not_set();
  test_fork_uidgid_match_parent();
  test_fork_getppid();


  printf(1, "\n*****  Completed PKproctests *****\n");

  exit();
}