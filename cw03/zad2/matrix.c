#include <stdio.h>
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
  FILE* file;
  uint rows, cols;
} matrix;

static number get(matrix* m, uint row, uint col) {
  number value;
  fseek(m->file, (m->cols * row + col) * sizeof(number), SEEK_SET);
  fread(&value, sizeof(number), 1, m->file);
  return value;
}

static void set(matrix* m, number value, uint row, uint col) {
  fseek(m->file, (m->cols * row + col) * sizeof(number), SEEK_SET);
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
  if (!file) panic("Can not read file '%s': %s", path, strerror(errno));
  matrix* m = malloc(sizeof(matrix));
  fseek(file, -2 * sizeof(uint), SEEK_END);
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

  number* clean_row = calloc(cols, sizeof(number));
  for (int row = 0; row < rows; row++) 
    fwrite(clean_row, sizeof(number), cols, file);

  uint size[2] = { rows, cols };
  fwrite(size, sizeof(uint), 2, file);

  fflush(file);
  fseek(file, 0, SEEK_SET);
  return m;
}

void free_matrix(matrix* m) {
  fclose(m->file);
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