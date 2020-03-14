#ifndef std
#define std

#include "util.c"

static int check_word_size_c(char* file) {
  FILE* f = fopen(file, "r");
  if (!f) panic("Can not open file '%s': %s", file, strerror(errno));

  int size = -1;
  for (char c = ' '; c && c != '\n' && c != '\r'; size++)
    fread(&c, sizeof(char), 1, f);
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

static void swap_c(FILE* file, int line1, int line2) {
  int step_size = word_size + new_line_size;
  char* buffer1 = malloc(sizeof(char) * word_size);
  char* buffer2 = malloc(sizeof(char) * word_size);

  if (fseek(file, line1 * step_size, SEEK_SET) < 0) 
    panic("Can not seek: %s", strerror(errno));
  if (fread(buffer1, sizeof(char), word_size, file) < 0) 
    panic("Can not read: %s", strerror(errno));

  if (fseek(file, line2 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (fread(buffer2, sizeof(char), word_size, file) < 0)
    panic("Can not read: %s", strerror(errno));

  if (fseek(file, line1 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (fwrite(buffer2, sizeof(char), word_size, file) < 0)
    panic("Can not write: %s", strerror(errno));
  
  if (fseek(file, line2 * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (fwrite(buffer1, sizeof(char), word_size, file) < 0)
    panic("Can not write: %s", strerror(errno));

  free(buffer1);
  free(buffer2);
}

static int partition_c(FILE* file, uint low, uint high) {
  uint step_size = word_size + new_line_size;

  char* pivot_buffer = malloc(sizeof(char) * word_size);
  char* buffer = malloc(sizeof(char) * word_size);

  if (fseek(file, high * step_size, SEEK_SET) < 0)
    panic("Can not seek: %s", strerror(errno));
  if (fread(pivot_buffer, sizeof(char), word_size, file) < 0)
    panic("Can not read: %s", strerror(errno));

  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (fseek(file, j * step_size, SEEK_SET) < 0)
      panic("Can not seek: %s", strerror(errno));
    if (fread(buffer, sizeof(char), word_size, file) < 0)
      panic("Can not read: %s", strerror(errno));
    
    if (strncmp(buffer, pivot_buffer, word_size) < 0) 
      swap_c(file, ++i, j);
  }
  swap_c(file, i + 1, high);

  free(pivot_buffer);
  free(buffer);
  return i + 1;
}

static void sort_c_(FILE* file, int low, int high) {
  if (low >= high) return;
  int pivot = partition_c(file, low, high);
  printf("%d\n", pivot);
  sort_c_(file, low, pivot - 1);
  sort_c_(file, pivot + 1, high);
}

void sort_c(char *file, int word_count_, int word_size_) {
  FILE* f = fopen(file, "r+");
  if (!f) panic("Can not open file '%s': %s", file, strerror(errno));

  word_count = word_count_;
  word_size = word_size_;

  sort_c_(f, 0, word_count - 1);

  fclose(f);
}

#endif