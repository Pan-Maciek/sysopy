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

typedef double number;
typedef unsigned int uint;

typedef struct matrix {
  FILE* file;
  uint rows, cols;
} matrix;

const long size_offset = 2 * sizeof(uint);
static number get(matrix* m, uint row, uint col) {
  number value;
  fseek(m->file, (m->cols * row + col) * sizeof(number) + size_offset, SEEK_SET);
  fread(&value, sizeof(number), 1, m->file);
  return value;
}

static void set(matrix* m, number value, uint row, uint col) {
  fseek(m->file, (m->cols * row + col) * sizeof(number) + size_offset, SEEK_SET);
  fwrite(&value, sizeof(number), 1, m->file);
}

void print_matrix(matrix* m) {
  number value;
  for (uint row = 0; row < m->rows; row++) {
    for (uint col = 0; col < m->cols; col++) {
      value = get(m, row, col);
      printf("%lf ", value);
    }
    printf("\n");
  }
}

matrix* open_matrix(char* path) {
  FILE* file = fopen(path, "r+");
  if (!file) return NULL;
  matrix* m = malloc(sizeof(matrix));
  fread(&m->rows, sizeof(uint), 1, file);
  fread(&m->cols, sizeof(uint), 1, file);

  m->file = file;
  return m;
}

matrix* create_matrix(char* path, uint rows, uint cols) {
  FILE* file = fopen(path, "w+");
  if (!file) return NULL;
  matrix* m = malloc(sizeof(matrix));
  m->file = file;
  m->rows = rows;
  m->cols = cols;

  fwrite(&m->rows, sizeof(uint), 1, file);
  fwrite(&m->cols, sizeof(uint), 1, file);

  number* clean_row = calloc(cols, sizeof(number));
  for (int row = 0; row < rows; row++) 
    fwrite(clean_row, sizeof(number), cols, file);

  fflush(file);
  fseek(file, 0, SEEK_SET);
  return m;
}

void free_matrix(matrix* m) {
  fclose(m->file);
  free(m);
}

matrix* multiply(matrix* A, matrix* B, char* out, int threads) {
  matrix* C = create_matrix(out, A->rows, B->cols);

  // spawn workers
  for (int i = 0; i < threads; i++) {
    if (fork() == 0) { // child
      exit(0);
    }
  }

  // wait for all to finish
  for (int i = 0; i < threads; i++)
    wait(NULL);

  return C;
}


int main() {
  int rows = 2, cols = 3;
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

  print_matrix(A);
  free_matrix(A);
  printf("\n");
  print_matrix(B);
  free_matrix(B);
  printf("\n");

  matrix* C = multiply(A, B, "C", 1);
  print_matrix(C);
  free_matrix(C);

  return 0;
}