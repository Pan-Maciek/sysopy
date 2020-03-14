#ifndef sys
#define sys

#include "util.c"

static int check_word_size_sys(char *file) {
  int fd = open(file, O_RDONLY), size = -1;
  if (fd < 0) panic("Can not open file '%s': %s", file, strerror(errno));

  for (char c = ' '; c && c != '\n' && c != '\r'; size++)
    read(fd, &c, 1);
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

static void swap_sys(int fd, int line1, int line2) {
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

static int partition_sys(int fd, uint low, uint high) {
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

static void sort_sys_(int fd, int low, int high) {
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

#endif