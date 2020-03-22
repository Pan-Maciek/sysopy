#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <errno.h>

#include "worker.c"
#define list_file argv[1]

int main(int argc, char** argv) {

  int processes = atoi(argv[2]), status;
  bool use_flock = strcmp(argv[4], "flock") == 0;
  pid_t pid;

  for (int i = 0; i < processes; i++) 
    if (fork() == 0) worker(list_file, i, processes, use_flock);
  
  while((pid = wait(&status)) != -1) 
    printf("pid(%i) status(%i)\n", pid, WEXITSTATUS(status));

  return 0;
}