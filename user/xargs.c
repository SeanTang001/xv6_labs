#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

char whitespace[] = " \t\r\n\v";
char all_lines[MAXARG][100];
char all_input[MAXARG][100];


int
main(int argc, char *argv[])
{
  int i;
  if (argc < 1){
    fprintf(2,"xargs: not enough arguments");
    exit(1);
  }

  char* argv_new[MAXARG];

  for (i = 1; i < argc; i++){
    argv_new[i-1] = argv[i];
  }

  char buf[100];

  i = 0;

  while(1){
    gets(buf, 100);
    if (buf[0] == 0){
      break;
    }
    strcpy(all_lines[i], buf);
    i=i+1;
  }


  for (int p = 0;p<i;p++){
    int j=0; 
    int ctr=0;
    for (int g = 0;g<strlen(all_lines[p]);g++){
        // if space or NULL found, assign NULL into newString[ctr]
        if(all_lines[p][g]==' '||all_lines[p][g]=='\0' || all_lines[p][g] == '\n')
        {
            all_input[ctr][j]='\0';
            ctr++;  //for next word
            j=0;    //for next word, init index to 0
        }
        else
        {
            all_input[ctr][j]=all_lines[p][g];
            j++;
        }
      }

    for (int l = 0; l <= ctr; l++){
      argv_new[argc-1+l] = all_input[l];
    }
    argv_new[ctr+argc] = 0;
    int pid = fork();
    if (pid == 0){
      exec(argv_new[0], argv_new);
      exit(0);
    }else{
      wait(0);
    }
  }
  exit(0);
}

