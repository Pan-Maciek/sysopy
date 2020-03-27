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
  printf("  %5s  │   %5s  │", CHILD_STATUS, PARENT_STATUS);\
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
#define PARENT_STATUS parent_ok ? "true" : "false"

bool parent_ok, child_ok;
void on_parent_ok(int _) { parent_ok = true; }
void on_child_ok(int _) { child_ok = true; }

int main() {
  signal(PARENT_OK, on_parent_ok);
  signal(CHILD_OK, on_child_ok);
  printf("                       │ fork() │       ignore       │      chandler      │        mask        │       pending      │\n");
  printf("┌────┬─────────────────┴────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┤\n");
  printf("│ no │                   signal │  child  │  parent  │  child  │  parent  │  child  │  parent  │  child  │  parent  │\n");
  printf("├────┼──────────────────────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┤\n");
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    printf("│ %2i │ %24s │", signal, strsignal(signal));
    static char* flags_fork[] = { "-i", "-h", "-m", "-p"};
    for (int i = 0; i < 4; i++) 
      exec_and_wait(EXE, "-s", sig, flags_fork[i]);
    printf("\n");
  }
  printf("└────┴──────────────────────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┘\n\n\n");

  printf("                       │ exec() │       ignore       │        mask        │       pending      │\n");
  printf("┌────┬─────────────────┴────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┤\n");
  printf("│ no │                   signal │  child  │  parent  │  child  │  parent  │  child  │  parent  │\n");
  printf("├────┼──────────────────────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┤\n");
  for (int signal = 1; signal <= 22; signal++) {
    char* sig = itoa(signal);
    printf("│ %2i │ %24s │", signal, strsignal(signal));
    static char* flags_exec[] = { "-i", "-m", "-p"};
    for (int i = 0; i < 3; i++) 
      exec_and_wait(EXE, "-e", "-s", sig, flags_exec[i]);
    printf("\n");
  }
  printf("└────┴──────────────────────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┘\n");
}