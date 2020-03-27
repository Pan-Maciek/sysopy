#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

pid_t pid;
#define repeat(n) for (int i = 0; i < n; i++)
bool wait = true;

int recived = 0;
void on_sig1(int _) { recived++; }
void on_sig2(int sig, siginfo_t * info, void * ucontext) {
  printf("[catcher] recived: %d\n", recived);
  pid = info->si_pid;
  wait = false;
}

union sigval val = {.sival_ptr=NULL};
int main(int _, char** argv) {
  bool use_sigrt = strcmp(argv[1], "sigrt") == 0;
  bool use_sigqueue = strcmp(argv[1], "sigqueue") == 0;
  bool use_kill = strcmp(argv[1], "kill") == 0;

  int SIG1 = use_sigrt ? SIGRTMIN : SIGUSR1;
  int SIG2 = use_sigrt ? SIG1 + 1 : SIGUSR2;

  signal(SIG1, on_sig1);
  struct sigaction act;
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = on_sig2;
  sigaction(SIG2, &act, NULL);

  printf("%d\n", getpid());

  while(wait);

  if (use_kill || use_sigrt) {
    repeat(recived) kill(pid, SIG1);
    kill(pid, SIG2);
  }
  else if (use_sigqueue) {
    repeat(recived) sigqueue(pid, SIG1, val);
    sigqueue(pid, SIG2, val);
  }

}
