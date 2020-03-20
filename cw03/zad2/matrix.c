#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define panic(str, args...) {\
  fprintf(stderr, "%s:%i ", __func__, __LINE__);\
  fprintf(stderr, str, args);\
  fprintf(stderr, ".\n");\
  exit(1);\
}

typedef int number;
typedef unsigned int uint;
#define number_size 12

typedef struct matrix {
  int file;
  uint rows, cols;
} matrix;

static int get_index(matrix* m, uint row, uint col) {
  return sizeof(char) * (m->cols * row + col) * number_size;
}

static number get(matrix* m, uint row, uint col) {
  static char raw[number_size];
  lseek(m->file, get_index(m, row, col), SEEK_SET);
  read(m->file, raw, (number_size-1) * sizeof(char));
  return atoi(raw);
}

static void set(matrix* m, number value, uint row, uint col) {
  lseek(m->file, get_index(m, row, col), SEEK_SET);
  dprintf(m->file, "%11i", value);
}

void print_matrix(matrix* m) {
  number value;
  for (uint row = 0; row < m->rows; row++) {
    for (uint col = 0; col < m->cols; col++) {
      value = get(m, row, col);
      printf("%i ", value);
    }
    printf("\n");
  }
}

matrix* open_matrix(char* path) {
  int file = open(path, O_RDWR);
  if (file < 0) panic("Can not read file '%s': %s", path, strerror(errno));
  matrix* m = malloc(sizeof(matrix));

  static char raw[number_size];

  lseek(file, -2 * number_size + 1, SEEK_END);
  read(file, raw, number_size * sizeof(char));
  raw[number_size - 1] = 0;

  m->rows = atoi(raw);
  read(file, raw, (number_size - 1) * sizeof(char));
  m->cols = atoi(raw);

  m->file = file;
  return m;
}

matrix* create_matrix(char* path, uint rows, uint cols) {
  int file = open(path, O_RDWR | O_CREAT | O_TRUNC);
  if (!file) panic("Can not read file '%s': %s", path, strerror(errno));
  matrix* m = malloc(sizeof(matrix));
  m->file = file;
  m->rows = rows;
  m->cols = cols;

  int row_size = (cols * number_size) * sizeof(char);
  char* clean_row = malloc(row_size);
  for (int i = 0; i < cols * number_size; i++)
    clean_row[i] = '\t';
  clean_row[row_size - 1] = '\n';

  for (int row = 0; row < rows; row++) 
    write(file, clean_row, row_size);
  free(clean_row);

  dprintf(file, "%11u\t%11u", rows, cols);

  lseek(file, 0, SEEK_SET);
  return m;
}

void free_matrix(matrix* m) {
  close(m->file);
  free(m);
}

int mul(matrix* A, matrix* B, matrix* C, int min_col, int max_col) {
  int inner = A->cols, sum, i;
  for (int row = 0; row < C->rows; row++) {
    for (int col = min_col; col < max_col; col++) {
      for (sum = i = 0; i < inner; i++) 
        sum += get(A, row, i) * get(B, i, col);
      set(C, sum, row, col);
    }
  }
  return 0; // number of multiplications done
}