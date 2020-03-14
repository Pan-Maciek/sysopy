#ifndef util
#define util

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

typedef unsigned int uint;

static char* buffer = 0;
static uint buffer_size = 0;
static uint word_size, word_count;
static const uint new_line_size = 1;

#define min(a, b) a > b ? b : a
#define max(a, b) a > b ? a : b

#define use(type, var_name, count, code_block){\
  type* var_name = calloc(count, sizeof(type));\
  uint var_name##_size = count;\
  code_block\
  free(var_name);\
}

#define panic(str, args...) {\
  fprintf(stderr, "%s:%i ", __func__, __LINE__);\
  fprintf(stderr, str, args);\
  fprintf(stderr, ".\n");\
  exit(1);\
}

#define arg(i, default) argc <= i ? default : argv[i]

void generate_word(char *word_buffer, uint word_size) {
  for (uint i = 0; i < word_size; i++)
    word_buffer[i] = 'a' + (rand() % ('z' - 'a'));
}

void generate(char *file, uint word_size, uint word_count) {
  int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC);
  if (fd < 0) panic("Can not open file '%s': %s", file, strerror(errno));

  use(char, word_buffer, word_size + 1, {
    word_buffer[word_size] = '\n';
    for (uint i = 0; i < word_count; i++) {
      generate_word(word_buffer, word_size);
      write(fd, word_buffer, word_buffer_size);
    }
  });

  close(fd);
}

#endif