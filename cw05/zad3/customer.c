#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"

int main(int argc, char** argv) {
  if (argc != 4) panic("Invalid number of arguments (recived %d; expected 3).", argc - 1);
  FILE* fifo = fopen(argv[1], "r");
  if (!fifo) panic("Can not open file '%s': %s", argv[1], strerror(errno));
  FILE* file = fopen(argv[2], "w+");
  if (!file) panic("Can not open file '%s': %s", argv[2], strerror(errno));
  int N = atoi(argv[3]), read;

  char* buffer = malloc(N * sizeof(char));
  while((read = fread(buffer, sizeof(char), N, fifo)) > 0) {
    fwrite(buffer, sizeof(char), read, file);
    fflush(file);
  }
}
