#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define min(a, b) a > b ? b : a
#define max(a, b) a > b ? a : b

#define panic(str, args...) {\
  fprintf(stderr, "%s:%i ", __func__, __LINE__);\
  fprintf(stderr, str, args);\
  fprintf(stderr, ".\n");\
  exit(1);\
}

typedef int number; // if changed remember to change dprint format
typedef unsigned int uint;
#define number_size 12

typedef struct matrix {
  int fd;
  uint rows, cols;
  number* values;
} matrix;

static int get_index(matrix* m, uint row, uint col) {
  return m->cols * row + col;
}

matrix* create_matrix(char* file, uint rows, uint cols) {
  matrix* m = malloc(sizeof(matrix));
  m->rows = rows;
  m->cols = cols;
  m->values = calloc(rows * cols, sizeof(number));
  int fd = m->fd = open(file, O_RDWR | O_CREAT | O_TRUNC);

  flock(fd, LOCK_EX);
  int row_size = cols * number_size;
  char* clean_row = malloc(row_size * sizeof(char));
  for (int i = 0; i < row_size; i++)
    clean_row[i] = '\t';
  clean_row[row_size - 1] = '\n';

  for (int row = 0; row < rows; row++)
    write(fd, clean_row, row_size * sizeof(char));
  free(clean_row);
  dprintf(fd, "%11u\t%11u\n", rows, cols);
  
  flock(fd, LOCK_UN);
  return m;
}

matrix* open_partial(int fd, uint min_col, uint max_col, uint rows, uint cols) {
  matrix* m = malloc(sizeof(matrix));

  m->rows = rows;
  m->cols = max_col - min_col + 1;

  m->values = malloc(m->rows * m->cols * sizeof(number));
  int row_size = number_size * cols * sizeof(char);
  int row_data_size = m->cols * number_size * sizeof(char);
  char* row_data = malloc(row_data_size);

  for (int row = 0; row < m->rows; row++) {
    lseek(fd, row_size * row + min_col * number_size * sizeof(char), SEEK_SET);
    read(fd, row_data, row_data_size);
    for (int i = number_size - 1; i <= row_data_size; i += number_size)
      row_data[i] = 0;
    char* tmp_row_ptr = row_data;
    for (int col = 0; col < m->cols; col++) {
      m->values[get_index(m, row, col)] = atoi(tmp_row_ptr);
      tmp_row_ptr += number_size;
    }
  }

  m->fd = fd;
  return m;
}

void read_size(int fd, uint* rows, uint* cols) { // flock file first
  static char raw[number_size];
  lseek(fd, -2 * number_size * sizeof(char), SEEK_END);
  read(fd, raw, number_size * sizeof(char));
  raw[number_size - 1] = 0;

  *rows = atoi(raw);
  read(fd, raw, (number_size - 1) * sizeof(char));
  *cols = atoi(raw);
}

matrix* open_matrix(char* file) {
  int fd = open(file, O_RDONLY);

  flock(fd, LOCK_EX);
  uint rows, cols;
  read_size(fd, &rows, &cols);
  matrix* m = open_partial(fd, 0, cols - 1, rows, cols);
  flock(fd, LOCK_UN);
  return m;
}

void print_matrix(matrix* m) {
  number value;
  for (uint row = 0; row < m->rows; row++) {
    for (uint col = 0; col < m->cols; col++) {
      value = m->values[get_index(m, row, col)];
      printf("%i ", value);
    }
    printf("\n");
  }
}

void free_matrix(matrix* m) {
  close(m->fd);
  free(m->values);
  free(m);
}

matrix* multiply(matrix* A, matrix* B) { // matrix B should be loaded using load_partial
  matrix* C = malloc(sizeof(matrix));
  C->rows = A->rows;
  C->cols = B->cols;
  C->values = malloc(C->rows * C->cols * sizeof(number));
  int inner = B->rows, sum, i;
  for (int row = 0; row < A->rows; row++) {
    for (int col = 0; col < B->cols; col++) {
      for (sum = i = 0; i < inner; i++)
        sum += A->values[get_index(A, row, i)] * B->values[get_index(B, i, col)];
      C->values[get_index(C, row, col)] = sum;
    }
  }
  return C;
}