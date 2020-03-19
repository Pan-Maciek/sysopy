#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/times.h>
#include <wait.h>

#include "matrix.c"

matrix* multiply(matrix* A, matrix* B, char* out, int workers) {
  matrix* C = create_matrix(out, A->rows, B->cols);

  // spawn workers
  for (int i = 0; i < A->cols; i+=2) {
    if (fork() == 0) { // child
      mul(A, B, C, i, i+2);
      exit(0);
    }
  }

  // wait for all to finish
  for (int i = 0; i < workers; i++)
    wait(NULL);

  return C;
}


int main() {
  int rows = 4, cols = 8;
  matrix* A = create_matrix("A", rows, cols);
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      set(A, col + row + 1, row, col);
    }
  }
  matrix* B = create_matrix("B", cols, cols);
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < cols; row++) {
      set(B, col + row + 1, row, col);
    }
  }

  matrix* C = multiply(A, B, "C", 3);

  print_matrix(A);
  free_matrix(A);
  printf("\n");
  print_matrix(B);
  free_matrix(B);
  printf("\n");
  print_matrix(C);
  free_matrix(C);

  return 0;
}