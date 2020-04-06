#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100
char buffer[BUFFER_SIZE];

static char* __tmp;
#define sprintfn(additional_size, pattern, args...) \
  (sprintf(__tmp = calloc(strlen(pattern) + additional_size, sizeof(char)), pattern, args), __tmp)

int main(int argc, char** argv) {
  if (argc != 2) return 1;
  int read;
  char* program = sprintfn(strlen(argv[1]), "sort %s", argv[1]);
  FILE* file = popen(program, "r");
  while((read = fread(buffer, sizeof(char), BUFFER_SIZE, file)))
    write(STDOUT_FILENO, buffer, read);
}
