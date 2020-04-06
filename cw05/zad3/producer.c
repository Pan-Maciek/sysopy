#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "common.h"

int main(int argc, char** argv) {
  if (argc != 4) panic("Invalid number of arguments (recived %d; expected 3).", argc - 1);
  FILE* fifo = fopen(argv[1], "r+");
  if (!fifo) panic("Can not open file '%s': %s", argv[1], strerror(errno));
  FILE* file = fopen(argv[2], "r");
  if (!file) panic("Can not open file '%s': %s", argv[2], strerror(errno));
  int N = atoi(argv[3]), read;
  srand(time(0));

  char* buffer = calloc(N + 1, sizeof(char));
  pid_t pid = getpid();
  while((read = fread(buffer, sizeof(char), N, file)) > 0) {
    sleep(rand() % 5 + 1);
    buffer[read] = 0;
    fprintf(fifo, "#%d#%s\n", pid, buffer);
    fflush(fifo);
  }
}
