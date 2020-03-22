#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <linux/limits.h>

#include "matrix.c"

// list_file    argv[1]
// timelimit    argv[2]
// use_flock    argv[3]
// worker_id    argv[4]
// worker_count argv[5]

static char 
A_file_path[PATH_MAX],
B_file_path[PATH_MAX], 
C_file_path[PATH_MAX];

int main(int argc, char** argv) {
  int id = atoi(argv[4]);
  int workers = atoi(argv[5]);

  FILE* list = fopen(argv[1], "r");
  int multiplied = 0;
  while (fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    int b_fd = open(B_file_path, O_RDONLY);
    uint rows, cols;
    read_size(b_fd, &rows, &cols);

    int cols_to_process = cols / workers + (cols & workers ? 1 : 0);
    int min_col = cols_to_process * id;
    int max_col = min(cols_to_process + min_col, cols - 1) - 1;
    if (max_col < min_col) {
      close(b_fd);
      continue;
    }
    matrix* A = open_matrix(A_file_path);
    matrix* B = open_partial(b_fd, min_col, max_col, rows, cols);
    matrix* C = multiply(A, B);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    multiplied++;
  }
  fclose(list);

  return multiplied;
}