#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>

int main(int argc, char** argv) {
  printf("list_file(%s) timelimit(%s) use_flock(%s)\n", argv[1], argv[2], argv[3]);
  return 0;
}