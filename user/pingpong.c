// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

	int status;
	// p[0] is read descriptor
	// p[1] is write descriptor
  int p1[2];
  int p2[2];

  pipe(p1);
  pipe(p2);

	int pid = fork();

	if (pid == 0){ // Child
		char* buffer[1];
		read(p1[1], buffer, 1);
		fprintf(1, "%d: received ping\n", getpid());
		write(p2[0], buffer, 1);
		exit(0);
	}else{ // Parent
		char* buffer[1];
		buffer[0] =  "a";
		write(p1[0], buffer, 1);
		wait(&status);
		read(p2[1],buffer, 1);
		sleep(1);
		fprintf(1, "%d: received pong\n", getpid());
	}

  exit(0);
}

