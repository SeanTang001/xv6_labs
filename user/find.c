#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find_file(char *path, char* target)
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

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    if (strcmp(path, target) == 0){
      fprintf(1, "%s", target);
    }
    else{
      fprintf(2, "find: '%s': No such file", target);
    }
    break;
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: path too long\n");
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
        fprintf(2, "ls: cannot stat %s\n", buf);
        continue;
      }
      if (st.type == 1){
        if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0){
          find_file(buf, target);
        }
      }
      else if (strcmp(p, target) == 0){
        printf("%s\n", buf);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    find_file(".", argv[1]);
    exit(0);
  }

  else if(argc == 3){
    find_file(argv[1], argv[2]);
    exit(0);
  }

  fprintf(2, "usage: find [path...] [expression]\n");

  exit(1);
}