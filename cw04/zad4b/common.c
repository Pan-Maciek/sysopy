#include <unistd.h>
#include <signal.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define listen \
  sigset_t mask;\
  sigprocmask(SIG_SETMASK, &mask, NULL);
#define to(p) pid = p;
#define then
#define and ;

static pid_t pid;
static sigset_t send_mask;
static bool waiting;
static union sigval val = {.sival_int = 0};

int received;
void count(int sig, siginfo_t* info, void* ucontext) {
  received++;
  sigqueue(info->si_pid, SIGUSR1, val);
}

void finish(int sig, siginfo_t* info, void* ucontext) {
  pid = info->si_pid;
  waiting = false;
  sigqueue(info->si_pid, SIGUSR1, val);
}

#define post \
  struct sigaction act = {\
    .sa_flags = SA_SIGINFO,\
    .sa_sigaction = ignore_sig };\
  sigaction(SIGUSR1, &act, NULL);\
  sigset_t post_mask;\
  sigaddset(&post_mask, SIGUSR1);\
  sigprocmask(SIG_SETMASK, &post_mask, NULL);\

void ignore_sig(int sig, siginfo_t* info, void* ucontext) {}
#define repeat(n) for (int i = 0; i < n; i++)
#define send(sig) {\
  kill(pid, sig);\
  sigemptyset(&send_mask);\
  sigsuspend(&send_mask); }

#define on(signal) { int sig = signal; struct sigaction act = {.sa_flags=SA_SIGINFO};
#define do(action) act.sa_sigaction = action; sigaction(sig, &act, NULL); }
#define receive { waiting = true; while(waiting); }