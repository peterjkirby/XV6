#include "types.h"
#include "user.h"
#include "param.h"

int
isValid(char *mode)
{
  if (strlen(mode) != 4) return 0;


  char c = *mode;
  // validate setuid value
  if (c != '0' && c != '1')
    return 0;

  // valdidate user, group owner
  ++mode;
  while (*mode) {
    c = *mode;
    // bad argument
    if (c < '0' || c > '7')
      return 0;

    ++mode;
  }

  return 1;
}

int main (int argc, char *argv[])
{
  char *mode;
  char *path;
  int rc;

  if (argc != 3) {
    printf(2, "Invalid usage. Use: chmod mode file\n");
    exit();
  }

  mode = argv[1];
  path = argv[2];

  if (!isValid(mode)) {
    printf(2, "Invalid mode. Please specify a mode value matching the format [0-1][0-7][0-7][0-7]. For example: 0755\n");
    exit();
  }

  rc = chmod(mode, path);
  if (rc) {
    switch (rc) {
      case ENOENT:
        printf(2, "No such file or directory\n");
        break;
      case EINVAL:
        printf(2, "Invalid usage. Use: chmod mode file\n");
        break;
      default:
        printf(2, "An unknown error occurred.\n");
    }
  }

  exit();
}