#include "common.c"

int main(int argc, char** argv) {
  bool use_sigqueue = is_arg(3, "sigqueue");
  bool use_sigrt    = is_arg(3, "sigrt");
  bool use_kill     = is_arg(3, "kill");

  pid_t pid = atoi(argv[1]);
  int sig_usr1_total = atoi(argv[2]);

  init(use_sigrt);

  if (use_kill or use_sigrt) {
    repeat(sig_usr1_total) kill(pid, SIG1);
    kill(pid, SIG2);
  }
  else if (use_sigqueue) {
    repeat(sig_usr1_total) sigqueue(pid, SIG1, val);
    sigqueue(pid, SIG2, val);
  }

  recive then 
    if (use_sigqueue)  printf("[sender]  recived: %d (catcher %d)\n", recived, sent);
    else printf("[sender]  recived: %d\n", recived);
}
