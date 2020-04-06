#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv) {
  FILE* fifo = fopen(argv[1], "w");
  FILE* file = fopen(argv[2], "r");
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
