#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define panic(str, args...) {\
  fprintf(stderr, "%s:%i ", __func__, __LINE__);\
  fprintf(stderr, str, args);\
  fprintf(stderr, ".\n");\
  exit(1);\
}

typedef double number;
typedef unsigned int uint;

typedef struct matrix {
  int file;
  uint rows, cols;
} matrix;

static number get(matrix* m, uint row, uint col) {
  number value;
  lseek(m->file, (m->cols * row + col) * sizeof(number), SEEK_SET);
  read(m->file, &value, sizeof(number));
  return value;
}

static void set(matrix* m, number value, uint row, uint col) {
  lseek(m->file, (m->cols * row + col) * sizeof(number), SEEK_SET);
  write(m->file, &value, sizeof(number));
}

void print_matrix(matrix* m) {
  number value;
  for (uint row = 0; row < m->rows; row++) {
    for (uint col = 0; col < m->cols; col++) {
      value = get(m, row, col);
      printf("%.2lf ", value);
    }
    printf("\n");
  }
}

matrix* open_matrix(char* path) {
  int file = open(path, O_RDWR);
  if (file < 0) panic("Can not read file '%s': %s", path, strerror(errno));
  matrix* m = malloc(sizeof(matrix));

  uint size[2]; // reduce sys calls

  lseek(file, -2 * sizeof(uint), SEEK_END);
  read(file, size, 2 * sizeof(uint));
  m->rows = size[0];
  m->cols = size[1];

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

  number* clean_row = calloc(cols, sizeof(number));
  for (int row = 0; row < rows; row++) 
    write(file, clean_row, cols * sizeof(number));
  free(clean_row);

  uint size[2] = { rows, cols };
  write(file, size, 2 * sizeof(uint));

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