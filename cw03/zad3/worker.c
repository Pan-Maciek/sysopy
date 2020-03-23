#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <wait.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <linux/limits.h>

#include "matrix.c"

char 
A_file_path[PATH_MAX],
B_file_path[PATH_MAX], 
C_file_path[PATH_MAX];

void worker(char* list_file, uint id, uint workers, bool use_flock) {
  FILE* list = fopen(list_file, "r");
  int multiplied = 0;
  int ipc = open("ipc", O_CREAT | O_RDWR, 0644);
  lseek(ipc, id * sizeof(int), SEEK_SET);
  write(ipc, &multiplied, sizeof(int));

  while (fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    int b_fd = open(B_file_path, O_RDONLY, 0644);
    uint rows, cols;
    read_size(b_fd, &rows, &cols);

    int cols_to_process = cols / workers + (cols % workers ? 1 : 0);
    int min_col = cols_to_process * id;
    int max_col = min(cols_to_process + min_col, cols - 1) - 1;
    if (max_col < min_col) {
      close(b_fd);
      continue;
    }

    matrix* A = open_matrix(A_file_path);
    matrix* B = open_partial(b_fd, min_col, max_col, rows, cols);
    matrix* C = multiply(A, B);

    if (use_flock) {
      int fd = open(C_file_path, O_CREAT | O_RDWR, 0644);
      dump_to_file(fd, min_col, cols, id, C);
      close(fd);
    }
    else dump_to_fragment_file(C_file_path, cols, id, C);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    multiplied++;
    lseek(ipc, id * sizeof(int), SEEK_SET);
    write(ipc, &multiplied, sizeof(int));
  }

  exit((multiplied << 1) | 1);
}

void time_manager(char* list_file, uint id, uint workers, bool use_flock, uint time_limit, uint mem_limit) {
  struct rlimit cpu, mem;
  cpu.rlim_cur = time_limit;
  cpu.rlim_max = time_limit;
  mem.rlim_cur = mem_limit;
  mem.rlim_max = mem_limit;
  setrlimit(RLIMIT_CPU, &cpu);
  setrlimit(RLIMIT_AS,  &mem);

  worker(list_file, id, workers, use_flock);
}