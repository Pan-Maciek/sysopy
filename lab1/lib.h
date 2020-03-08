#ifndef lib
#define lib

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define use(type, var_name, count, code_block){\
  type var_name = calloc(sizeof(type), count);\
  code_block\
  free(var_name);\
}

#define use_malloc(type, var_name, size, code_block){\
  type var_name = malloc(size);\
  code_block\
  free(var_name);\
}

typedef struct block {
  unsigned int size, count;
  char* files[2];
  char** operations;
} block;

typedef struct table {
  unsigned int size, count;
  block** values;
} table;

table* create_table(int size);
block* compare_files(char* file1, char* file2);

#endif