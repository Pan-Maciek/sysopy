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

int check_word_size_sys(char *file) {
  int fd = open(file, O_RDONLY), size = 0;
  if (fd < 0) panic("Can not open file '%s': %s", file, strerror(errno));
  char c = ' ';
  while(c && c != '\n' && c != '\r') {
    read(fd, &c, 1);
    size++;
  }
  close(fd);
  return size;
}

void copy_sys(char *source, char *target, uint word_count) {
  int word_size = check_word_size_sys(source), size;
  uint bytes_to_copy = word_count * (word_size + new_line_size);

  int in = open(source, O_RDONLY);
  if (in < 0) panic("Can not open file '%s': %s", source, strerror(errno));
  
  int out = open(target, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (out < 0) panic("Can not open file '%s': %s", target, strerror(errno));

  while (bytes_to_copy > 0 && (size = read(in, buffer, min(buffer_size, bytes_to_copy)))) {
    bytes_to_copy -= size;
    write(out, buffer, size);
  }

  close(in);
  close(out);
}

int check_word_size_c(char* file) {
  FILE* f = fopen(file, "r");
  if (!f) panic("Can not open file '%s': %s", file, strerror(errno));

  char c = ' ';
  int size = 0;
  while(c && c != '\n' && c != '\r') {
    fread(&c, sizeof(char), 1, f);
    size++;
  }
  fclose(f);
  return size;
}

void copy_c(char *source, char *target, uint word_count) {
  int word_size = check_word_size_c(source), size;
  uint bytes_to_copy = word_count * (word_size + new_line_size);

  FILE* in = fopen(source, "r");
  if (!in) panic("Can not open file '%s': %s", source, strerror(errno));

  FILE* out = fopen(target, "w");
  if (!out) panic("Can not open file '%s': %s", target, strerror(errno));

  while(bytes_to_copy > 0 && (size = fread(buffer, sizeof(char), min(buffer_size, bytes_to_copy), in))) {
    bytes_to_copy -= size;
    fwrite(buffer, sizeof(char), size, out);
  }

  fclose(in);
  fclose(out);
}

void swap_sys(int fd, int line1, int line2) {
  int step_size = word_size + new_line_size;
  char* buffer1 = malloc(sizeof(char) * word_size);
  char* buffer2 = malloc(sizeof(char) * word_size);

  if (lseek(fd, line1 * step_size, SEEK_SET) < 0) 
    panic("Can not seek: %s", strerror(errno));
  if (read(fd, buffer1, word_size) < 0) 
    panic("Can not read: %s", strerror(errno));

  if (lseek(fd, line2 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (read(fd, buffer2, word_size) < 0)
    panic("Can not read: %s", strerror(errno));

  if (lseek(fd, line1 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (write(fd, buffer2, word_size) < 0)
    panic("Can not write: %s", strerror(errno));
  
  if (lseek(fd, line2 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (write(fd, buffer1, word_size) < 0)
    panic("Can not write: %s", strerror(errno));

  free(buffer1);
  free(buffer2);
}

int partition_sys(int fd, uint low, uint high) {
  uint step_size = word_size + new_line_size;

  char* pivot_buffer = malloc(sizeof(char) * word_size);
  char* buffer = malloc(sizeof(char) * word_size);

  if (lseek(fd, high * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (read(fd, pivot_buffer, word_size) < 0)
    panic("Can not read: %s", strerror(errno));

  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (lseek(fd, j * step_size, SEEK_SET) < 0)
      panic("Can not seek: %s", strerror(errno));
    if (read(fd, buffer, word_size) < 0)
      panic("Can not read: %s", strerror(errno));
    
    if (strncmp(buffer, pivot_buffer, word_size) < 0) 
      swap_sys(fd, ++i, j);
  }
  swap_sys(fd, i + 1, high);

  free(pivot_buffer);
  free(buffer);
  return i + 1;
}

void sort_sys_(int fd, int low, int high) {
  if (low >= high) return;
  int pivot = partition_sys(fd, low, high);
  sort_sys_(fd, low, pivot - 1);
  sort_sys_(fd, pivot + 1, high);
}

void sort_sys(char *file, int word_count_, int word_size_) {
  int fd = open(file, O_RDWR);
  if (fd < 0) panic("Can not open file '%s': %s", file, strerror(errno));

  word_count = word_count_;
  word_size = word_size_;

  sort_sys_(fd, 0, word_count - 1);

  close(fd);
}

int main(int argc, char** argv) {
  if (strcmp(argv[1], "generate") == 0) {
    int word_count = atoi(argv[3]);
    int word_size = atoi(argv[4]);
    generate(argv[2], word_count, word_size);
  }

  else if (strcmp(argv[1], "sort") == 0) {
    char* file = argv[2];
    int word_count = atoi(argv[3]);
    int word_size = atoi(argv[4]);
    bool use_sys = strcmp(argv[5], "sys") == 0;

    if (use_sys) sort_sys(file, word_count, word_size);
  }

  else if (strcmp(argv[1], "copy") == 0) {
    char* source = argv[2];
    char* target = argv[3];
    int word_count = atoi(argv[4]);
    buffer_size = atoi(arg(5, "512"));
    bool use_sys = strcmp(arg(6, "sys"), "sys") == 0;
    buffer = malloc(buffer_size * sizeof(char));

    if (use_sys) copy_sys(source, target, word_count);
    else copy_c(source, target, word_count);
  } 
}