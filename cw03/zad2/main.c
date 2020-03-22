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

  if (use_flock) return 0;

  // merge files with paste
  FILE* list = fopen(list_file, "r");
  int out = fileno(stdout);
  while(fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    int fd = open(B_file_path, O_RDONLY);
    uint cols;
    read_size(fd, NULL, &cols);

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
    fd = open(C_file_path, O_RDWR | O_CREAT | O_TRUNC);
    dup2(fd, out);
    if (fork() == 0) execvp("paste", argv);
    if (fork() == 0) execvp("rm", argv);

    for (int i = 1; i <= argc; i++) free(argv[i]);
    free(argv);

    close(fd);
  }

  return 0;
}