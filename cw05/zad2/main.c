#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if (argc != 2) return 1;
  dup2(fileno(popen("sort", "w")), STDOUT_FILENO);
  execlp("cat", "cat", argv[1], NULL);
}
