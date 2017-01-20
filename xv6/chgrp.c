#include "types.h"
#include "user.h"
#include "param.h"


int main (int argc, char *argv[])
{

  char *path;
  int gid;
  int rc;

  if (argc != 3) {
    printf(2, "Invalid usage. Use: chgrp gid file\n");
    exit();
  }

  gid = atoi(argv[1]);
  path = argv[2];

  if (gid <= 0 || 32767 < gid) {
    printf(2, "Invalid gid. Specify a value in the range 0 < gid <= 32767\n");
  } else {
    rc = chgrp(gid, path);
    if (rc) {
      switch (rc) {
        case ENOENT:
          printf(2, "No such file or directory\n");
          break;
        case EINVAL:
          printf(2, "Invalid usage. Use: chgrp gid file\n");
          break;
        default:
          printf(2, "An unknown error occurred.\n");
      }
    }
  }

  exit();
}