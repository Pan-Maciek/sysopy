#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <errno.h>

#define list_file argv[1]
#define timelimit argv[3]
#define use_flock argv[4]
#define worker_exe "worker.out"

int main(int argc, char** argv) {

  int processes = atoi(argv[2]), status;
  pid_t pid;

  char process_id[12], processes_count[12];
  sprintf(processes_count, "%i", processes);
  for (int i = 0; i < processes; i++) 
    if (fork() == 0) {
      sprintf(process_id, "%i", i);
      execl(worker_exe, worker_exe, list_file, timelimit, use_flock, process_id, processes_count, NULL);
    }
  
  while((pid = wait(&status)) != -1) 
    printf("pid(%i) status(%i)\n", pid, WEXITSTATUS(status));

  return 0;
}