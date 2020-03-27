#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

pid_t pid;
#define send(sig) kill(pid, sig)
bool wait = false;

void on_sigusr1(int _){}
void on_sigusr2(int _){
  wait = false;
}

int main(int _, char** argv) {
  signal(SIGUSR1, on_sigusr1);
  signal(SIGUSR2, on_sigusr2);

  pid = atoi(argv[1]);
  int sig_usr1_total = atoi(argv[2]);

  for (int i = 0; i < sig_usr1_total; i++)
    send(SIGUSR1);
  send(SIGUSR2);

  wait = true;
  while(wait);
}