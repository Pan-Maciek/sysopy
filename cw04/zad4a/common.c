#include <unistd.h>
#include <signal.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int recived = 0, sent;
bool wait = true;
pid_t pid;
union sigval val = {.sival_int=0};

void on_sig1(int _) { recived++; }
void on_sig2(int sig, siginfo_t * info, void * ucontext) {
  pid = info->si_pid;
  wait = false;
  sent = info->si_value.sival_int;
}

#define init(use_sigrt) \
int SIG1 = use_sigrt ? SIGRTMIN : SIGUSR1;\
int SIG2 = use_sigrt ? SIG1 + 1 : SIGUSR2;\
signal(SIG1, on_sig1);\
struct sigaction act = {\
  .sa_flags = SA_SIGINFO,\
  .sa_sigaction = on_sig2\
};\
sigaction(SIG2, &act, NULL);

#define repeat(n) for (int i = 0; i < n; i++)
#define is_arg(n, s) (argc > n && (strcmp(argv[n], s) == 0))
#define recive while(wait);
#define then
#define or ||