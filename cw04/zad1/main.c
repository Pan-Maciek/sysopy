#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#define repeat for(;;)
#define foreach(var, f) for (__auto_type var = f; var; var = f)
#define not(val) (!(val))

#define on(sig, fn) void ON_##sig fn
#define no(sig) signal(sig, ON_##sig)
#define no_block(sig) {\
  struct sigaction act = {.sa_handler=ON_##sig, .sa_flags=SA_NODEFER};\
  sigemptyset(&act.sa_mask);\
  sigaction(sig, &act, NULL);}

static DIR* dir;

on(SIGTSTP, (int sig) {
  static bool wait = false;
  if not (wait = !wait) return;
  static const char message[] = "\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu.\n";
  write(STDOUT_FILENO, message, sizeof(message));
  pause();
})

on(SIGINT, (int sig) {
  static const char message[] = "\nOdebrano sygnał SIGINT.\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(0);
})

int main(int argc, char** argv) {
  no_block(SIGTSTP);
  no(SIGINT);

  repeat {
    dir = opendir(".");
    foreach(entry, readdir(dir))
      printf("%s\n", entry->d_name);
    closedir(dir);
  }
}