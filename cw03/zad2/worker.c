#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <linux/limits.h>

#include "matrix.c"

char 
A_file_path[PATH_MAX],
B_file_path[PATH_MAX], 
C_file_path[PATH_MAX];

void worker(char* list_file, uint id, uint workers, bool use_flock) {
  FILE* list = fopen(list_file, "r");
  int multiplied = 0;
  while (fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    int b_fd = open(B_file_path, O_RDONLY);
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
      int fd = open(C_file_path, O_CREAT | O_RDWR);
      dump_to_file(fd, min_col, cols, id, C);
      close(fd);
    }
    else dump_to_fragment_file(C_file_path, cols, id, C);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    multiplied++;
  }
  fclose(list);

  exit(multiplied);
}