#ifndef lib
#define lib

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define use(type, var_name, count, code_block){\
  type* var_name = calloc(count, sizeof(type));\
  code_block\
  free(var_name);\
}

#define use_malloc(type, var_name, size, code_block){\
  type var_name = malloc(size);\
  code_block\
  free(var_name);\
}

typedef struct block {
  unsigned int size;
  char* files[2];
  char** operations;
} block;

typedef struct table {
  unsigned int size;
  block** values;
} table;

table* create_table(int size);
block* compare_files(char* file1, char* file2);
void compare_file_sequence(table* table, char** file_sequence);
void remove_block(table* table, int block_index);
void remove_operation(table* table, int block_index, int operation_index);

int operation_count(table* table, int block_index);
int blocks_count(table* table);

#endif
