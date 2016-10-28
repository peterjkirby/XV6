#include "types.h"
#include "user.h"

#define MAX_ARGS 20

static void
copy_args(int argc, char *argv[], char *cpy[])
{
  int i;
  for (i = 0; i < argc && i < MAX_ARGS; ++i)
    cpy[i] = argv[i];
}

int main (int argc, char *argv[])
{
  int start_t;
  int stop_t;
  int p[2];
  char *prog_argv[MAX_ARGS];

  // if the user specifies too many arguments, do not run the program.
  if (MAX_ARGS < (argc - 1)) {
    printf(1, "\nToo many arguments specified. Cannot run specified program. \n");
    exit();
  }

  if (argc == 1) {
    // no program specified
    printf(1, "\n ran in 0.0 seconds\n");
  } else {
    copy_args(argc - 1, argv + 1, prog_argv);
    
    pipe(p);
    if (fork() == 0) {
      start_t = uptime();
      write(p[1], &start_t, sizeof(int));
      exec(prog_argv[0], prog_argv);
      exit();
    } else {
      read(p[0], &start_t, sizeof(int));
      wait();
      stop_t = uptime();

      int el = stop_t - start_t;
      pkprintf(1, "\n%s ran in %f%f seconds\n", prog_argv[0], el / 100, el % 100);
    }
  }


  exit();
}