#include "common.c"

int main(int argc, char** argv) {
  bool use_sigqueue = is_arg(1, "sigqueue");
  bool use_sigrt    = is_arg(1, "sigrt");
  bool use_kill     = is_arg(1, "kill");

  if (argc < 3) printf("%d\n", getpid());

  init(use_sigrt);
  receive then printf("[catcher] received: %d\n", received);

  if (use_kill or use_sigrt) {
    repeat(received) kill(pid, SIG1);
    then kill(pid, SIG2);
  }
  else if (use_sigqueue) {
    repeat(received) sigqueue(pid, SIG1, val); 
    val.sival_int = received;
    then sigqueue(pid, SIG2, val);
  }
}
