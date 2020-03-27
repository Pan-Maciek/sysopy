#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>

#define exec_and_wait(program, args...) {\
  child_ok = parent_ok = false;\
  if (fork() == 0) execl(program, program, args, NULL);\
  waitpid(WAIT_ANY, &status, WUNTRACED);\
}

#define EXE "./sig_test.out"
char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}
static int status;

#define PARENT_OK SIGUSR1
#define CHILD_OK SIGUSR2
#define CHILD_STATUS child_ok ? "true" : "false"
#define PARENT_STATUS child_ok ? "true" : "false"

bool parent_ok, child_ok;
void on_parent_ok(int _) { parent_ok = true; }
void on_child_ok(int _) { child_ok = true; }

int main() {
  signal(PARENT_OK, on_parent_ok);
  signal(CHILD_OK, on_child_ok);
  printf("                                │       ignore       │      chandler      │\n");
  printf("┌────┬──────────────────────────┼─────────┬──────────┼─────────┬──────────┤\n");
  printf("│ no │                   signal │  child  │  parent  │  child  │  parent  │\n");
  printf("├────┼──────────────────────────┼─────────┼──────────┼─────────┼──────────┤\n");
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    exec_and_wait(EXE, "-s", sig, "-i");
    printf("│ %2i │ %24s │  %5s  │   %5s  │", signal, strsignal(signal), CHILD_STATUS, PARENT_STATUS);

    exec_and_wait(EXE, "-s", sig, "-h");
    printf("  %5s  │   %5s  │\n", CHILD_STATUS, PARENT_STATUS);
  }
  printf("└────┴──────────────────────────┴─────────┴──────────┴─────────┴──────────┘\n");
}