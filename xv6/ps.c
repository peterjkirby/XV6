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


	// display the table header
	pkprintf(1, "\n%-6%s %-6%s %-6%s %-6%s %-6%s %-15%s %-15%s %-10%s %-15%s %-15%s\n", "PID", "UID", "GID", "PPID", "Pri", "Elapsed (s)", "CPU Time (s)", "State", "Size (bytes)", "Name");
		
	for (int i = 0; i < UTABLE_SIZE; ++i) {
		if (strcmp(up->state, "unused") == 0)
			break;
		
		pkprintf(1, "%-6%d %-6%d %-6%d %-6%d %-6%d %-15%f%f %-15%f%f %-10%s %-15%d %-15%s\n", 
      up->pid, 
      up->uid, 
      up->gid, 
      up->ppid,
	  up->priority,
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
