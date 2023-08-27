// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exec_pipe(int fd){
    int n;
    int p;
    read(fd, &n, 4);

    fprintf(1, "prime %d\n", n);

    int p1[2];

    pipe(p1);
    int flag=-1;
    while(read(fd, &p, 4) != 0){
        flag=1;
        if (p%n != 0){
            write(p1[1], &p, 4);
        }
    }
    if(flag == -1){
        close(fd);
        close(p1[0]);
        close(p1[1]);
        return;
    }

    int pid = fork();

    if (pid != 0){
        close(fd);
        close(p1[1]);
        close(p1[0]);
        wait(0);
    }else{
        close(fd);
        close(p1[1]);
        exec_pipe(p1[0]);
        close(p1[0]);
    }
    exit(0);
}



int
main(int argc, char *argv[])
{


    int p1[2];

    pipe(p1);

    for (int i=2;i<35;i++){
        int n = i;
        write(p1[1], &n, 4);
    }
    close(p1[1]);
    exec_pipe(p1[0]);
    close(p1[0]);

    exit(0);
}

