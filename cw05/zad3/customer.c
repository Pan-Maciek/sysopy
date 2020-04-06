#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
  FILE* fifo = fopen(argv[1], "r");
  FILE* file = fopen(argv[2], "w+");
  int N = atoi(argv[3]), read;

  char* buffer = calloc(N, sizeof(char));
  while((read = fread(buffer, sizeof(char), N, fifo)) > 0) {
    fwrite(buffer, sizeof(char), read, file);
    fflush(file);
  }
}
