#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int usr1_count = 0;
pid_t pid;
bool wait = true;
void on_sigusr1(int _) { usr1_count++; }
void on_sigusr2(int sig, siginfo_t * info, void * ucontext) {
  printf("recived: %d\n", usr1_count);
  pid = info->si_pid;
  wait = false;
}
int main() {
  signal(SIGUSR1, on_sigusr1);
  struct sigaction act;
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = on_sigusr2;
  sigaction(SIGUSR2, &act, NULL);

  printf("%d\n", getpid());

  while(wait);

  for (int i = 0; i < usr1_count; i++)
    kill(pid, usr1_count);
  kill(pid, SIGUSR2);
}