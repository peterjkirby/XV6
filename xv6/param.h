#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       2000  // size of file system in blocks
#define MAX_PRIORITY 7
#define DEFAULT_UID 1
#define DEFAULT_GID 1
#define DEFAULT_MKFS_P_DIR_MODE 0644
#define DEFAULT_MKFS_P_FILE_MODE 0755
#define DEFAULT_P_MODE 0600

#define P_EXEC 0x00000001
#define P_WRITE 0x00000002
#define P_READ 0x00000004

#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define EACCES          13      /* Permission denied */
#define EINVAL          22      /* Invalid argument */