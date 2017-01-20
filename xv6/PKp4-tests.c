#include "types.h"
#include "user.h"
#include "param.h"
#include "stat.h"
#include "print_mode.c"

#define TEST_FILE "testsetuid"

#define NUM_VALID_PERMS 11
char *VALID_PERMS[] = {
        "0777",
        "0700",
        "0070",
        "0007",
        "0000",
        "0200",
        "0020",
        "0002",
        "1544",
        "1454",
        "1445"
};

#define NUM_INVALID_PERMS 4
char *INVALID_PERMS[] = {
        "0800",
        "0888",
        "2000",
        "0009",
};

void
reset()
{
  chown(DEFAULT_UID, TEST_FILE);
  chgrp(DEFAULT_GID, TEST_FILE);
  chmod("755", TEST_FILE);
}

void
log_info(char *testName, struct stat *st)
{
  printf(1, "%s - file[name='%s', uid=%d, gid=%d] \n", testName, TEST_FILE, st->uid, st->gid);
}

void
test_valid_chown()
{
  int rc;
  struct stat st;
  int newUid;

  stat(TEST_FILE, &st);

  log_info("test_valid_chown", &st);

  newUid = getuid() + 1;
  printf(1, "test_valid_chown - running `chown(%d, '%s')` now\n", newUid, TEST_FILE);


  chown(newUid, TEST_FILE);
  stat(TEST_FILE, &st);

  if (st.uid == newUid) {
    printf(1, "test_valid_chown [PASS] - %s uid expected=%d, actual=%d\n", TEST_FILE, newUid, st.uid);
  } else {
    printf(1, "test_valid_chown [FAIL] - %s uid expected=%d, actual=%d\n", TEST_FILE, newUid, st.uid);
  }

  reset();
}

void
test_chown_nonexistant_file()
{
  int rc;
  int newUid;

  newUid = getuid() + 1;
  printf(1, "test_chown_nonexistant_file - running `chown(%d, '%s')` now\n", newUid, "nonexistant-file");

  rc = chown(newUid, "nonexistant-file");

  if (rc) {
    printf(1, "test_chown_nonexistant_file [PASS] - chown return code was %d, expected -1\n", rc);
  } else {
    printf(1, "test_chown_nonexistant_file [FAIL] - chown return code was %d, expected -1\n", rc);
  }

}

void
test_chown_bad_uid()
{
  int rc;
  struct stat st;
  int newUid;
  int oriUid;

  stat(TEST_FILE, &st);

  oriUid = st.uid;
  log_info("test_chown_bad_uid", &st);

  newUid = -1;
  printf(1, "test_chown_bad_uid - running `chown(%d, '%s')` now\n", newUid, TEST_FILE);


  rc = chown(newUid, TEST_FILE);
  stat(TEST_FILE, &st);

  if (rc) {
    if (st.uid == newUid) {
      printf(1, "test_chown_bad_uid [FAIL] - chown return code was %d, but %s uid expected=%d, actual=%d\n", rc, TEST_FILE, oriUid, st.uid);
    } else {
      printf(1, "test_chown_bad_uid [PASS] - chown return code was %d, %s uid expected=%d, actual=%d\n", rc, TEST_FILE, oriUid, st.uid);
    }
  } else {
    printf(1, "test_chown_bad_uid [FAIL] - chown return code was %d, %s uid expected=%d, actual=%d\n", rc, TEST_FILE, oriUid, st.uid);
  }

  reset();
}

void
test_valid_chgrp()
{
  int rc;
  struct stat st;
  int newGid;

  stat(TEST_FILE, &st);
  log_info("test_valid_chgrp", &st);
  newGid = getgid() + 1;
  printf(1, "test_valid_chgrp - running `chgrp(%d, '%s')` now\n", newGid, TEST_FILE);


  chgrp(newGid, TEST_FILE);
  stat(TEST_FILE, &st);

  if (st.gid == newGid) {
    printf(1, "test_valid_chgrp [PASS] - %s gid expected=%d, actual=%d\n", TEST_FILE, newGid, st.gid);
  } else {
    printf(1, "test_valid_chgrp [FAIL] - %s gid expected=%d, actual=%d\n", TEST_FILE, newGid, st.gid);
  }

  reset();
}

void
test_chgrp_nonexistant_file()
{
  int rc;
  int newGid;

  newGid = getgid() + 1;
  printf(1, "test_chgrp_nonexistant_file - running `chgrp(%d, '%s')` now\n", newGid, "nonexistant-file");


  rc = chgrp(newGid, "nonexistant-file");

  if (rc) {
    printf(1, "test_chgrp_nonexistant_file [PASS] - chgrp return code was %d, expected -1\n", rc);
  } else {
    printf(1, "test_chgrp_nonexistant_file [FAIL] - chgrp return code was %d, expected -1\n", rc);
  }

}

void
test_chgrp_bad_gid()
{
  int rc;
  struct stat st;
  int newGid;
  int oriGid;

  stat(TEST_FILE, &st);

  oriGid = st.gid;
  log_info("test_chgrp_bad_gid", &st);

  newGid = -1;
  printf(1, "test_chgrp_bad_gid - running `chgrp(%d, '%s')` now\n", newGid, TEST_FILE);

  rc = chgrp(newGid, TEST_FILE);
  stat(TEST_FILE, &st);

  if (rc) {
    if (st.gid == newGid) {
      printf(1, "test_chgrp_bad_gid [FAIL] - chgrp return code was %d, but %s gid expected=%d, actual=%d\n", rc, TEST_FILE, oriGid, st.gid);
    } else {
      printf(1, "test_chgrp_bad_gid [PASS] - chgrp return code was %d, %s gid expected=%d, actual=%d\n", rc, TEST_FILE, oriGid, st.gid);
    }
  } else {
    printf(1, "test_chgrp_bad_gid [FAIL] - chgrp return code was %d, %s gid expected=%d, actual=%d\n", rc, TEST_FILE, oriGid, st.gid);
  }

  reset();
}

void
toOct(uint mode, char *octStr)
{
  int i = 3;

  for (i = 0; i < 4; ++i)
    octStr[i] = '0';

  i = 3;
  while(mode > 0)
  {
    octStr[i] = '0' + (mode % 8);
    mode /= 8;
    --i;
  }

  octStr[4] = '\0';
}

void
test_valid_chmod()
{
  char octStr[5];
  int rc;
  struct stat st;
  uint mode;

  stat(TEST_FILE, &st);
  mode = st.mode.asInt;

  for (int i = 0; i < NUM_VALID_PERMS; ++i) {
    printf(1, "test_valid_chmod - running chmod('%s', '%s')\n", VALID_PERMS[i], TEST_FILE);
    chmod(VALID_PERMS[i], TEST_FILE);
    stat(TEST_FILE, &st);
    toOct(st.mode.asInt, octStr);
    if (mode == st.mode.asInt) {
      printf(1, "test_valid_chmod [FAIL] - %s mode expected = %s, actual %s\n", TEST_FILE, VALID_PERMS[i], octStr);
    } else {
      printf(1, "test_valid_chmod [PASS] - %s mode expected = %s, actual %s\n", TEST_FILE, VALID_PERMS[i], octStr);
    }
  }

  reset();
}

void
test_chmod_nonexistant_file()
{
  int rc;
  struct stat st;

  stat(TEST_FILE, &st);

  printf(1, "test_chmod_nonexistant_file - running `chmod(%s, '%s')` now\n", "0777", "nonexistant-file");


  rc = chmod("0777", "nonexistant-file");

  if (rc) {
    printf(1, "test_chmod_nonexistant_file [PASS] - chmod return code was %d, expected -1\n", rc);
  } else {
    printf(1, "test_chmod_nonexistant_file [FAIL] - chmod return code was %d, expected -1\n", rc);
  }

}

void
test_chmod_bad_mode()
{
  char expectOctStr[5];
  char octStr[5];
  int rc;
  struct stat st;
  uint mode;

  stat(TEST_FILE, &st);
  mode = st.mode.asInt;
  toOct(mode, expectOctStr);

  for (int i = 0; i < NUM_INVALID_PERMS; ++i) {
    printf(1, "test_chmod_bad_mode - running chmod('%s', '%s')\n", INVALID_PERMS[i], TEST_FILE);
    rc = chmod(INVALID_PERMS[i], TEST_FILE);
    stat(TEST_FILE, &st);
    toOct(st.mode.asInt, octStr);
    if (rc) {
      if (mode == st.mode.asInt) {
        printf(1, "test_chmod_bad_mode [PASS] - chmod return code expected=%d, actual=%d, %s mode expected = %s, actual %s\n", -1, rc, TEST_FILE, expectOctStr, octStr);
      } else {
        printf(1, "test_chmod_bad_mode [FAIL] - chmod return code expected=%d, actual=%d, %s mode expected = %s, actual %s\n", -1, rc, TEST_FILE, expectOctStr, octStr);
      }
    } else {
      printf(1, "test_chmod_bad_mode [FAIL] - chmod return code expected=%d, actual=%d, %s mode expected = %s, actual %s\n", -1, rc, TEST_FILE, expectOctStr, octStr);
    }

  }

  reset();
}


int
has_permission(struct stat *st, int mask)
{
  uint mode = st->mode.asInt;
  int uid = getuid();
  int gid = getgid();

  if (uid == st->uid) {
    mode >>= 6;
  } else {
    if (gid == st->gid) {
      mode >>= 3;
    }
  }

  if ((mask & mode) == mask)
    return 1;

  return 0;
}

int
test_exec_with()
{
  char *argv[] = {TEST_FILE, '\0'};
  //argv[0] = TEST_FILE;
  int rc;
  int success;
  int pid;
  int p[2];
  pipe(p);
  rc = 0;
  success = 0;

  pid = fork();
  if (pid == 0) {
    close(1);
    dup(p[1]);
    rc = exec(TEST_FILE, argv);
    write(p[1], &rc, sizeof(int));
    exit();
  } else if (pid < 0) {
    printf(1, "FORK FAILURE\n");
  } else {
    read(p[0], &success, sizeof(int));
    close(p[0]);
    close(p[1]);
    wait();
  }

  return success == -1;
}

void
test_exec_permissions()
{

  char modeStr[5];
  /*char *argv[1];
  argv[0] = TEST_FILE;*/

  int testNum = 1;
  int cpid;
  int rc;
  int gid = getgid();
  int uid = getuid();
  struct stat st;

  int failed;
/*  int p[2];
  pipe(p);*/


  for (int curGid = gid; curGid <= gid + 1; ++curGid) {
    for (int curUid = uid; curUid <= uid + 1; ++curUid) {

      for (int i = 0; i < NUM_VALID_PERMS; ++i) {
        //failed = 0;
        chown(curUid, TEST_FILE);
        chgrp(curGid, TEST_FILE);
        chmod(VALID_PERMS[i], TEST_FILE);

        stat(TEST_FILE, &st);
        toOct(st.mode.asInt, modeStr);
        printf(1, "test_exec_permissions[%d] - running exec on %s with %s[uid=%d, gid=%d, mode=%s] and proc[uid=%d, gid=%d]\n",
                testNum,
                TEST_FILE,
                TEST_FILE,
                st.uid,
                st.gid,
                modeStr,
                uid,
                gid);

        rc = test_exec_with(curUid, curGid, VALID_PERMS[i]);

        if (rc) {
          if (has_permission(&st, P_EXEC)) {
            printf(1, "test_exec_permissions[%d] [FAIL] - expected= exec to execute %s, actual= exec did not execute %s\n",testNum, TEST_FILE, TEST_FILE);
          } else {
            printf(1, "test_exec_permissions[%d] [PASS] - expected= exec not to execute %s, actual= exec did not execute %s\n", testNum,TEST_FILE, TEST_FILE);
          }
        } else {
          if (has_permission(&st, P_EXEC)) {
            printf(1, "test_exec_permissions[%d] [PASS] - expected= exec to execute %s, actual= exec executed %s\n",testNum, TEST_FILE, TEST_FILE);
          } else {
            printf(1, "test_exec_permissions[%d] [FAIL] - expected= exec not to execute %s, actual= exec executed %s\n", testNum,TEST_FILE, TEST_FILE);
          }
        }

        /*cpid = fork();
        printf(1, "cpid=>%d", cpid);
        if (cpid == 0) {
          exec(TEST_FILE, argv);
         // write(p[1], &rc, sizeof(int));
          exit();
        } else if (cpid < 0) {
          printf(1, "FORK FAILURE\n");
        } else {
          //read(p[0], &failed, sizeof(int));
          wait();
          stat(TEST_FILE, &st);
          toOct(st.mode.asInt, modeStr);
          *//*printf(1, "test_exec_permissions - running exec on %s with %s[uid=%d, gid=%d, mode=%s] and proc[uid=%d, gid=%d]\n",
                 TEST_FILE,
                 TEST_FILE,
                 st.uid,
                 st.gid,
                 modeStr,
                 uid,
                 gid);*//*
        *//*  if (failed) {
            if (has_permission(&st, P_EXEC)) {
              printf(1, "test_exec_permissions [FAIL] - expected= exec to execute %s, actual= exec did not execute %s\n", TEST_FILE, TEST_FILE);
            } else {
              printf(1, "test_exec_permissions [PASS] - expected= exec not to execute %s, actual= exec did not execute %s\n", TEST_FILE, TEST_FILE);
            }
          } else {
            if (has_permission(&st, P_EXEC)) {
              printf(1, "test_exec_permissions [PASS] - expected= exec to execute %s, actual= exec executed %s\n", TEST_FILE, TEST_FILE);
            } else {
              printf(1, "test_exec_permissions [FAIL] - expected= exec to execute %s, actual= exec executed %s\n", TEST_FILE, TEST_FILE);
            }
          }*//*
        }*/

        ++testNum;
      }

    }
  }

  //reset();

}

void
test_exec_setuid()
{
  char *argv[] = {TEST_FILE, '\0'};
  char modeStr[5];
  struct stat st;
  int uid = getuid();
  int gid = getgid();


  for (int i = 1; i <= 3; ++i) {
    for (int j = 1; j <= 3; ++j) {
      for (int m = 0; m <= 1; ++m) {
        chown(i, TEST_FILE);
        chgrp(j, TEST_FILE);

        if (m) {
          if (uid == i) {
            chmod("1544", TEST_FILE);
          } else if (gid == j) {
            chmod("1454", TEST_FILE);
          } else {
            chmod("1445", TEST_FILE);
          }
        } else {
          if (uid == i) {
            chmod("0544", TEST_FILE);
          } else if (gid == j) {
            chmod("0454", TEST_FILE);
          } else {
            chmod("0445", TEST_FILE);
          }
        }

        stat(TEST_FILE, &st);
        toOct(st.mode.asInt, modeStr);
        printf(1,
               "test_exec_setuid - running exec on %s with file[name=%s, uid=%d, gid=%d, mode=%s] and proc[uid=%d, gid=%d]\n",
               TEST_FILE,
               TEST_FILE,
               st.uid,
               st.gid,
               modeStr,
               uid,
               gid);

        int cpid = fork();
        if (cpid == 0) {
          exec(TEST_FILE, argv);
          exit();
        } else if (cpid < 0) {
          // fork failed :(
          printf(1, "FORK FAILED\n");
        } else {
          wait();
          if (m) {
            printf(1, "test_exec_setuid [VERIFY] - testsetuid uid should be equal to: %d\n", i);
          } else {
            printf(1, "test_exec_setuid [VERIFY] - testsetuid uid should be equal to: %d\n", uid);
          }
        }
      }

    }
  }
}

void
test_fstat()
{
  struct stat st;
  char modeStr[5];
  stat(TEST_FILE, &st);
  toOct(st.mode.asInt, modeStr);
  reset();
  printf(1, "test_fstat - file[name='%s', uid=%d, gid=%d, mode=%s] \n", TEST_FILE, st.uid, st.gid, modeStr);
  printf(1, "test_fstat - running `chown(%d, '%s')` now\n", 2, TEST_FILE);
  printf(1, "test_fstat - running `chgrp(%d, '%s')` now\n", 2, TEST_FILE);
  printf(1, "test_fstat - running `chmod(%s, '%s')` now\n", "0777", TEST_FILE);
  chown(2, TEST_FILE);
  chgrp(2, TEST_FILE);
  chmod("0777", TEST_FILE);
  stat(TEST_FILE, &st);
  toOct(st.mode.asInt, modeStr);

  printf(1, "test_fstat - running stat for file[name='%s'] \n", TEST_FILE);
  printf(1, "test_fstat - stat results: file[name='%s', uid=%d, gid=%d, mode=%s] \n", TEST_FILE, st.uid, st.gid, modeStr);
  if (st.uid == 2 && st.gid == 2 && strcmp(modeStr, "0777") == 0) {
    printf(1, "test_fstat [PASS] - fstat successfully displayed updated file information \n", 2, TEST_FILE);
  } else {
    printf(1, "test_fstat [FAIL] - fstat did not display the updated file information \n", 2, TEST_FILE);
  }

}

int
main(int argc, char *argv[])
{
  printf(1, "\n*****  Running PKp4-tests  *****\n");

  //test_valid_chown();
  //test_chown_nonexistant_file();
  //test_chown_bad_uid();

  //test_valid_chgrp();
  //test_chgrp_nonexistant_file();
  //test_chgrp_bad_gid();

  //test_valid_chmod();
  //test_chmod_nonexistant_file();
  //test_chmod_bad_mode();

  //test_fstat();

  //test_exec_permissions();

  test_exec_setuid();

  printf(1, "\n*****  Completed PKp4-tests *****\n");

  exit();
}
