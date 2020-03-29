#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>
#include "strings.h"
#define IPC_MAIN
#include "ipc.h"

#define exec_and_wait(program, args...) {\
  child_ok = parent_ok = false;\
  if (fork() == 0) execl(program, program, args, NULL);\
  waitpid(WAIT_ANY, NULL, WUNTRACED);\
  printf("  %5s  │   %5s  │", CHILD_STATUS, PARENT_STATUS);\
}

#define EXE "./sig_test.out"
char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}

const char* const flags_fork[] = { "-i", "-h", "-m", "-p"};
const char* const flags_exec[] = { "-i", "-m", "-p"};

int main() {
  IPC_INIT;

  printf(FORK_HEADER);
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    printf("│ %2i │ %24s │", signal, strsignal(signal));
    for (int i = 0; i < 4; i++) 
      exec_and_wait(EXE, "-s", sig, flags_fork[i]);
    printf("\n");
  }
  printf(FORK_FOOTER "\n\n");

  printf(EXEC_HEADER);
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    printf("│ %2i │ %24s │", signal, strsignal(signal));
    for (int i = 0; i < 3; i++) 
      exec_and_wait(EXE, "-e", "-s", sig, flags_exec[i]);
    printf("\n");
  }
  printf(EXEC_FOOTER);
}