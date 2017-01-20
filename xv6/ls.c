#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

#ifdef CS333_P4
#include "print_mode.c"
#endif

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

#ifdef CS333_P4
  pkprintf(1, "%-10%s %-15%s %-4%s %-4%s %-6%s %-8%s\n", "Mode", "Name", "uid", "gid", "inode", "size");
#endif

  switch(st.type){
  case T_FILE:
#ifdef CS333_P4
    print_mode(&st);
    pkprintf(1, " %-15%s %-4%d %-4%d %-6%d %-8%d\n", fmtname(path), st.uid, st.gid, st.ino, st.size);
#else
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
#endif
    break;
  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }

#ifdef CS333_P4
      print_mode(&st);
      pkprintf(1, " %-15%s %-4%d %-4%d %-6%d %-8%d\n", fmtname(buf), st.uid, st.gid, st.ino, st.size);
#else
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
#endif

    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
