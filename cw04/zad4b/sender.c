#include <signal.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include "common.c"

int main(int argc, char** argv) {
  pid_t pid = atoi(argv[1]);
    int n   = atoi(argv[2]);

  post to (pid) 
    repeat(n) send(SIGUSR1);
    then send(SIGUSR2);

  listen
    on (SIGUSR1) do (count)
    on (SIGUSR2) do (finish)

  then receive and printf("[sender]  %d\n", received);
}
