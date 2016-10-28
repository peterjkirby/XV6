// System call numbers
#define SYS_fork    1
#define SYS_exit    SYS_fork+1
#define SYS_wait    SYS_exit+1
#define SYS_pipe    SYS_wait+1
#define SYS_read    SYS_pipe+1
#define SYS_kill    SYS_read+1
#define SYS_exec    SYS_kill+1
#define SYS_fstat   SYS_exec+1
#define SYS_chdir   SYS_fstat+1
#define SYS_dup     SYS_chdir+1
#define SYS_getpid  SYS_dup+1
#define SYS_sbrk    SYS_getpid+1
#define SYS_sleep   SYS_sbrk+1
#define SYS_uptime  SYS_sleep+1
#define SYS_open    SYS_uptime+1
#define SYS_write   SYS_open+1
#define SYS_mknod   SYS_write+1
#define SYS_unlink  SYS_mknod+1
#define SYS_link    SYS_unlink+1
#define SYS_mkdir   SYS_link+1
#define SYS_close   SYS_mkdir+1
#define SYS_halt    SYS_close+1
// student system calls begin here. Follow the existing pattern.

// @version 1.1
#define SYS_date	SYS_halt+1

// @version 1.2
#define SYS_getuid	SYS_date+1
#define SYS_getgid	SYS_getuid+1
#define SYS_getppid	SYS_getgid+1
#define SYS_setuid	SYS_getppid+1
#define SYS_setgid	SYS_setuid+1
#define SYS_getprocs SYS_setgid+1

// @version 1.3
#define SYS_setpriority SYS_getprocs+1