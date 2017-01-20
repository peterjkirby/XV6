#include "types.h"
#include "user.h"
#include "param.h"


int main (int argc, char *argv[])
{

  char *path;
  int uid;
  int rc;

  if (argc != 3) {
    printf(2, "Invalid usage. Use: chown uid file\n");
    exit();
  }

  uid = atoi(argv[1]);
  path = argv[2];

  if (uid <= 0 || 32767 < uid) {
    printf(2, "Invalid uid. Specify a value in the range 0 < uid <= 32767\n");
  } else {
    rc = chown(uid, path);
    if (rc) {
      switch (rc) {
        case ENOENT:
          printf(2, "No such file or directory\n");
          break;
        case EINVAL:
          printf(2, "Invalid usage. Use: chown uid file\n");
          break;
        default:
          printf(2, "An unknown error occurred.\n");
      }
    }
  }

  exit();
}