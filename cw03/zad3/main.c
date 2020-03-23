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

long int usage_time(struct timeval * t){
  return (long int)t->tv_sec * 1000000 + (long int)t->tv_usec;
}

void report_usage(struct rusage usage1, struct rusage usage2){
  long int userTime = abs(usage_time(&usage2.ru_utime) - usage_time(&usage1.ru_stime));
  long int systemTime = abs(usage_time(&usage2.ru_stime) - usage_time(&usage1.ru_stime));
  printf("  user CPU time used: %lf\n", (double)userTime / 1000000);
  printf("system CPU time used: %lf\n\n", (double)systemTime / 1000000);
}

int main(int argc, char** argv) {

  int processes = atoi(argv[2]), status;
  int time_limit = atoi(argv[3]);
  int mem_limit = 1048576 * atoi(argv[4]);
  bool use_flock = strcmp(argv[5], "flock") == 0;
  pid_t pid;

  for (int id = 0; id < processes; id++) 
    if (fork() == 0) time_manager(list_file, id, processes, use_flock, time_limit, mem_limit);
  
  struct rusage usage1, usage2;
  getrusage(RUSAGE_CHILDREN, &usage1);
  while((pid = wait(&status)) != -1) {
    getrusage(RUSAGE_CHILDREN, &usage2);
    printf("pid(%i) status(%i)\n", pid, WEXITSTATUS(status));
    report_usage(usage1, usage2);
  }

  if (use_flock) return 0;

  // merge files with paste
  FILE* list = fopen(list_file, "r");
  int out = fileno(stdout);
  while(fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    int fd = open(B_file_path, O_RDONLY);
    uint rows, cols;
    read_size(fd, &rows, &cols);

    int cols_to_process = cols / processes + (cols % processes ? 1 : 0);
    int argc = cols / cols_to_process + (cols % cols_to_process ? 1 : 0);
    char** argv = malloc((argc + 2) * sizeof(char*));
    int len = strlen(C_file_path) + 14; // max_int size with sign is 12 + '-' + terminator
    argv[0] = "paste";
    for (int i = 1; i <= argc; i++) {
      argv[i] = calloc(len, sizeof(char));
      sprintf(argv[i], "%s-%i", C_file_path, i - 1);
    }
    close(fd);
    fd = open(C_file_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    dup2(fd, out);
    if (fork() == 0) execvp("paste", argv);
    if (fork() == 0) execvp("rm", argv);

    for (int i = 1; i <= argc; i++) free(argv[i]);
    free(argv);
    while(wait(NULL) != -1);
    dprintf(fd, "%11u\t%11u\n", rows, cols);

    close(fd);
  }

  return 0;
}
