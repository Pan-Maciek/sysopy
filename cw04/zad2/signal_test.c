#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <getopt.h>

#define foreach_ref(var, f) for (var; f >= 0;)
#define case(name, code) case opt_##name##_i: code; break

#define opt_signal_i  0
#define opt_ignore_i  1
#define opt_handler_i 2
#define opt_mask_i    3
#define opt_pending_i 4

static int sig, status;
static pid_t pid;
const struct option options[] = {
  {"signal" , required_argument, 0, 's'},
  {"ignore" , no_argument      , 0, 'i'},
  {"handler", no_argument      , 0, 'h'},
  {"mask"   , no_argument      , 0, 'm'},
  {"pending", no_argument      , 0, 'p'},
  {0, 0, 0, 0}
};

void test_ignore() {
  signal(sig, SIG_IGN);
  if ((pid = fork()) == 0) {
    raise(sig);
    exit(EXIT_SUCCESS);
  }
  raise(sig);
  wait(&status);
  exit(status);
}

int main(int argc, char** argv) {
  struct sigaction act;
  foreach_ref(int opt, getopt_long_only(argc, argv, "", options, &opt)) {
    switch (opt) {
      case(signal, sig = atoi(optarg));
      case(pending, {});
      case(handler, {});
      case(ignore, test_ignore());
      case(mask, {});
    }
  }
}