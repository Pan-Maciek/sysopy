#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>

#define exec_and_wait(program, args...) {\
  if (fork() == 0) execl(program, program, args, NULL);\
  waitpid(WAIT_ANY, &status, WUNTRACED);\
  status = WEXITSTATUS(status);\
  child_ok = status <= 3 && status & 2;\
  parent_ok = status <= 3 && status & 1;\
}

#define EXE "./sig_test.out"
#define btos(value) value ? "true" : "false"
char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}
static int status, child_ok, parent_ok;

int main() {
  printf("┌────┬──────────────────────────┬────────┬──────────┐\n");
  printf("│ no │                   signal │ ignore │ changler │\n");
  printf("├────┼──────────────────────────┼────────┼──────────┤\n");
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    exec_and_wait(EXE, "-s", sig, "-i");
    printf("│ %2i │ %24s │  %5s │", signal, strsignal(signal), btos(child_ok && parent_ok));

    exec_and_wait(EXE, "-s", sig, "-h");
    printf(" %8s │\n", btos(child_ok && parent_ok));
  }
  printf("└────┴──────────────────────────┴────────┴──────────┘\n");
}