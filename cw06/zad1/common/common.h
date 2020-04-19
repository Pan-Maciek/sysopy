#ifndef common_h
#define common_h

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#define SERVER_KEY_PATHNAME (getpwuid(getuid())->pw_dir)
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0620

#define def
#define event_loop void event_loop_fn()
#define init void init_fn(int argc, char **argv)
#define finish void finish_fn()
void init_fn(int argc, char **argv);
void finish_fn();
void event_loop_fn();

#define repeat for (;;)
#define foreach(var, ptr, from, length) \
  for (typeof(*ptr) *var = ptr, *last = ptr + length - 1; var <= last; ++var)

// ansi escape codes
#define left(n) "\033[" #n "D"
#define clearline(n) "\033[" #n "K"

#define red "\033[31m"
#define green "\033[32m"
#define yellow "\033[93m"
#define cyan "\033[96m"
#define cdefault "\033[0m"

#define print(x) write(STDOUT_FILENO, x, sizeof(x))

// rutime
void on_sigint(int _) {
  print(left(10) "Recived " red "SIGINT" cdefault " shutting down gracefully.\n");
  exit(0);
}

void at_exit() {
  finish_fn();
  print("Finish: " green "OK" cdefault "\n");
}

int main(int argc, char **argv) {
  print("Initializing");
  signal(SIGINT, on_sigint);
  atexit(at_exit);
  init_fn(argc, argv);
  print(left(100) "Init:" clearline() green "\tOK\n" cdefault);
  print("Event loop\n");
  event_loop_fn();
}

#endif