#include "common.c"

int main(int argc, char** argv) {
  on (SIGUSR1) do (count)
  on (SIGUSR2) do (finish)

  receive then 
    printf("[catcher] %d\n", received)
  and post
    repeat(received) send(SIGUSR1)
    then send(SIGUSR2)
}

