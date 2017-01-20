#include "types.h"
#include "user.h"
#include "uproc.h"

#define UTABLE_SIZE 100

int
main(int argc, char *argv[])
{

	struct uproc *utable;
	struct uproc *up;

	utable = malloc(sizeof(struct uproc) * UTABLE_SIZE);
	getprocs(UTABLE_SIZE, utable);
	up = utable;

#ifdef CS333_P3
	// display the table header
	pkprintf(1, "\n%-6%s %-6%s %-6%s %-6%s %-6%s %-10%s %-15%s %-15%s %-10%s %-15%s %-15%s\n", "PID", "UID", "GID", "PPID", "Pri", "Budget", "Elapsed (s)", "CPU Time (s)", "State", "Size (bytes)", "Name");
#else
	pkprintf(1, "\n%-6%s %-6%s %-6%s %-6%s %-15%s %-15%s %-10%s %-15%s %-15%s\n", "PID", "UID", "GID", "PPID", "Elapsed (s)", "CPU Time (s)", "State", "Size (bytes)", "Name");
#endif
	for (int i = 0; i < UTABLE_SIZE; ++i) {
		if (strcmp(up->state, "unused") == 0)
			break;
		
		pkprintf(1, "%-6%d %-6%d %-6%d %-6%d %-6%d %-10%d %-15%f%f %-15%f%f %-10%s %-15%d %-15%s\n",
      up->pid, 
      up->uid, 
      up->gid, 
      up->ppid,
#ifdef CS333_P3
	 		up->priority,
			up->budget,
#endif
      up->elapsed_ticks / 100, 
      up->elapsed_ticks % 100, 
      up->CPU_total_ticks / 100, 
      up->CPU_total_ticks % 100,
      up->state, up->size, up->name);


		++up;
	}

	free(utable);

	exit();
  return 0;
}
