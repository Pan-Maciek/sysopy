#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>

#define EXE "./sig_test.out"
#define btos(value) value ? "true" : "false"
char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}

int main() {
  pid_t pid;
  for (int signal = 1, status; signal <= 22; signal++) {
    if ((pid = fork()) == 0) execl(EXE, EXE, "-s", itoa(signal), "-i", NULL);
    waitpid(WAIT_ANY, &status, WUNTRACED);
    printf("signal(%i)\tignored(%s)\n", signal, btos(!status));
  }
}