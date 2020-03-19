#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>

#include "matrix.c"

bool  opt_show = false;
bool  opt_generate = false;
int   opt_rows = 0;
int   opt_cols = 0;
char* opt_file = NULL;

#define opt_show_i     0
#define opt_generate_i 1
#define opt_file_i     2

#define case(name, code)  case opt_##name##_i: code; break

const struct option options[] = {
  {"show"    , no_argument      , 0, 's'},
  {"generate", required_argument, 0, 'g'},
  {"file"    , required_argument, 0, 'f'},
  0
};

int main(int argc, char** argv) {
  int option, l;
  while((option = getopt_long_only(argc, argv, "", options, &l)) >= 0) {
    switch (l) {
      case(show, opt_show = true);
      case(generate, {
        opt_generate = true;
        opt_rows = atoi(strtok(optarg, "x"));
        opt_cols = atoi(strtok(NULL, "x"));
      });
      case(file, opt_file = optarg);
    }
  }

  if (opt_file == NULL) panic("-file is required", 0);

  matrix* m = opt_generate ? create_matrix(opt_file, opt_rows, opt_cols) : open_matrix(opt_file);
  if (opt_generate) {
    for (int row = 0; row < opt_rows; row++) 
      for (int col = 0; col < opt_cols; col++) 
         set(m, rand() % 10, row, col);
  }

  if (opt_show) print_matrix(m);
  return 0;
}