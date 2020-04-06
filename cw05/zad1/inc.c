#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int value, inc = argc == 2 ? atoi(argv[1]) : 1;

  while(scanf("%d", &value) > 0)
    printf("%d\n", value + inc);
}
