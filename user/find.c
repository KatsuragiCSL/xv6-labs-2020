#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// copy from ls.c then edit
void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (read(fd, &de, sizeof(de)) != sizeof(de)) {
    exit(1);
  }

  switch(st.type){
  case T_FILE:
    if (strcmp(de.name, name) == 0) {
      printf("%s/%s\n", path, name);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      // Don't recurse into "." and "..".
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || (strcmp(de.name, "..") == 0)) {
        continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if (st.type == T_FILE) {
        if (strcmp(de.name, name) == 0) {
          printf("%s/%s\n", path, name);
        }
      } else if (st.type == T_DIR) {
        // recurse
        find(buf, name);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  // first arg = path, second arg = filename
  char *path = argv[1];
  char *name = argv[2];

  find(path, name);
  exit(0);
}
