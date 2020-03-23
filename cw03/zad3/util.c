#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <linux/limits.h>

#include "matrix.c"
#undef min
#undef max

int   opt_min = 0;
int   opt_max = 0;
int   opt_count = 1;
char* opt_generate = NULL;

#define opt_generate_i 0
#define opt_min_i      1
#define opt_max_i      2
#define opt_count_i    3
#define opt_test_i     4

#define case(name, code)  case opt_##name##_i: code; break

char A_file_path[PATH_MAX], B_file_path[PATH_MAX], C_file_path[PATH_MAX]; 

const struct option options[] = {
  {"generate", required_argument, 0, 'g'},
  {"min"     , required_argument, 0, 'm'},
  {"max"     , required_argument, 0, 'M'},
  {"count"   , required_argument, 0, 'c'},
  {"test"    , required_argument, 0, 't'},
  {0, 0, 0, 0}
};

int test(char* list_file) {
  FILE* list = fopen(list_file, "r");
  while(fscanf(list, "%s %s %s\n", A_file_path, B_file_path, C_file_path) == 3) {
    matrix* a = open_matrix(A_file_path);
    matrix* b = open_matrix(B_file_path);
    matrix* c = open_matrix(C_file_path);

    matrix* d = multiply(a, b);
    bool matching = true;
    for (int row = 0; row < c->rows; row++)
      for (int col = 0; col < c->cols; col++)
        if (c->values[get_index(c, row, col)] != d->values[get_index(d, row, col)]) {
          matching = false;
          goto non;
        }
    non:
    printf("%s x %s = %s %s\n", A_file_path, B_file_path, C_file_path, matching ? "true" : "false");
  }
  return 0;
}

int rand_range(int min, int max) {
  return rand() % (max - min) + min;
}

void generate_file(char* path, uint rows, uint cols) {
  matrix* m = create_matrix(rows, cols);
  int fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644);
  for (int row = 0; row < rows; row++) 
    for (int col = 0; col < cols; col++) 
      m->values[get_index(m, row, col)] = rand_range(-100, 100);

  dump_to_file(fd, 0, cols, 0, m);
  close(fd);
}

int generate(char* list_file) {
  FILE* file = fopen(list_file, "w+");
  char path[30];
  for (uint i = 0; i < opt_count; i++) {
    uint Arows = rand_range(opt_min, opt_max), Acols = rand_range(opt_min, opt_max);
    sprintf(path, "data/A%u.txt", i);
    generate_file(path, Arows, Acols);
    fprintf(file, "%s ", path);

    uint Brows = Acols, Bcols = rand_range(opt_min, opt_max);
    sprintf(path, "data/B%u.txt", i);
    generate_file(path, Brows, Bcols);
    fprintf(file, "%s ", path);

    fprintf(file, "data/C%u.txt\n", i);
  }

  return 0;
}

int main(int argc, char** argv) {
  int option, l;
  while((option = getopt_long_only(argc, argv, "", options, &l)) >= 0) {
    switch (l) {
      case(generate, exit(generate(optarg)));
      case(min, opt_min = atoi(optarg));
      case(max, opt_max = atoi(optarg));
      case(test, exit(test(optarg)));
      case(count, opt_count = atoi(optarg));
    }
  }

  return 0;
}