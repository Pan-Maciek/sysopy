#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define send(sig) kill(pid, sig)
#define repeat(n) for (int i = 0; i < n; i++)
bool wait = false;

int recived = 0;
void on_sig1(int _) { recived++; }
void on_sig2(int _) { wait = false; }

union sigval val = {.sival_ptr=NULL};
int main(int _, char** argv) {
  bool use_sigrt = strcmp(argv[3], "sigrt") == 0;
  bool use_sigqueue = strcmp(argv[3], "sigqueue") == 0;
  bool use_kill = strcmp(argv[3], "kill") == 0;

  int SIG1 = use_sigrt ? SIGRTMIN : SIGUSR1;
  int SIG2 = use_sigrt ? SIG1 + 1 : SIGUSR2;

  signal(SIG1, on_sig1);
  signal(SIG2, on_sig2);

  pid_t pid = atoi(argv[1]);
  int sig_usr1_total = atoi(argv[2]);

  if (use_kill || use_sigrt) {
    repeat(sig_usr1_total) kill(pid, SIG1);
    kill(pid, SIG2);
  }
  else if (use_sigqueue) {
    repeat(sig_usr1_total) sigqueue(pid, SIG1, val);
    sigqueue(pid, SIG2, val);
  }


  wait = true;
  while(wait);

  printf(" [sender] recived: %d\n", recived);
}
