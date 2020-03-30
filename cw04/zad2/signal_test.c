#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <getopt.h>
#include "ipc.h"

#define foreach_ref(var, f) for (var; f >= 0;)
#define case(name, code) case opt_##name##_i: code; break
#define exec(EXE, args...) execl(EXE, EXE, args, NULL);

static int sig;
bool use_exec = false;
static pid_t ppid;
char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}

void test_ignore() {
  signal(sig, SIG_IGN);
  if (fork() == 0) {
    if (use_exec) exec("./exec.out", "-i", itoa(sig), itoa(ppid));
    raise(sig);
    kill(ppid, CHILD_OK);
  }
  waitpid(WAIT_ANY, NULL, WUNTRACED);
  raise(sig);
  kill(ppid, PARENT_OK);
}

int sig_handler_success = CHILD_OK;
void sig_handler(int _) { kill(ppid, sig_handler_success); }
void test_handler() {
  signal(sig, sig_handler);
  if (fork() == 0) raise(sig);
  waitpid(WAIT_ANY, NULL, WUNTRACED);
  sig_handler_success = PARENT_OK;
  raise(sig);
}

void failure_handler(int _) { _exit(0); }
void test_mask() {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, sig);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  signal(sig, failure_handler);
  if (fork() == 0) {
    if (use_exec) exec("./exec.out", "-m", itoa(sig), itoa(ppid));
    raise(sig);
    kill(ppid, CHILD_OK);
  }
  waitpid(WAIT_ANY, NULL, WUNTRACED);
  raise(sig);
  kill(ppid, PARENT_OK);
}

void test_pending() {
  sigset_t newmask;
  sigemptyset(&newmask);
  sigaddset(&newmask, sig);
  sigprocmask(SIG_SETMASK, &newmask, NULL);

  raise(sig);
  sigset_t mask;
  if (fork() == 0) {
    if (use_exec) exec("./exec.out", "-p", itoa(sig), itoa(ppid));
    sigpending(&mask);
    if (sigismember(&mask, sig)) 
      kill(ppid, CHILD_OK);
  }
  waitpid(WAIT_ANY, NULL, WUNTRACED);
  sigpending(&mask);
  if (sigismember(&mask, sig)) kill(ppid, PARENT_OK);
}

#define opt_signal_i  0
#define opt_ignore_i  1
#define opt_handler_i 2
#define opt_mask_i    3
#define opt_pending_i 4
#define opt_exec_i    5

const struct option options[] = {
  {"signal" , required_argument, 0, 's'},
  {"ignore" , no_argument      , 0, 'i'},
  {"handler", no_argument      , 0, 'h'},
  {"mask"   , no_argument      , 0, 'm'},
  {"pending", no_argument      , 0, 'p'},
  {"exec"   , no_argument      , 0, 'e'},
  {0, 0, 0, 0}
};

int main(int argc, char** argv) {
  ppid = getppid();
  foreach_ref(int opt, getopt_long_only(argc, argv, "", options, &opt)) {
    switch (opt) {
      case(signal , sig = atoi(optarg));
      case(exec   , use_exec = true);
      case(pending, test_pending());
      case(handler, test_handler());
      case(ignore , test_ignore());
      case(mask   , test_mask());
    }
  }
}