#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define spawn(command) if (fork() == 0) \
    for (char **__x = command;; (execvp(__x[0], __x) != -1) || (exit(1), 1))
#define wait_all while (wait(NULL) > 0) ;

#define redirect(x) dup2(x);
#define output(pipe) (close(pipe[1]), pipe[0])
#define input(pipe) (close(pipe[0]), pipe[1])
#define to ,
#define MAX_ARGS 10
#define unless(x) if (!(x))

char **parse(char *line) {
  static char *elements[MAX_ARGS + 1];

  int i = 0;
  while (line != NULL && i < MAX_ARGS) {
    char *arg = strsep(&line, " \n");
    if (*arg)
      elements[i++] = arg;
  }

  while (i < MAX_ARGS)
    elements[i++] = NULL;

  return elements;
}

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  FILE *commands = fopen(argv[1], "r");

  int current[2], previous[2] = {-1, -1};
  char *line = NULL, *process_info;
  size_t bsize = 0;
#define last line == NULL

  while (getline(&line, &bsize, commands) > 0) {
    while ((process_info = strsep(&line, "|")) != NULL) {
      pipe(current);
      spawn(parse(process_info)) {
        redirect(output(previous) to STDIN_FILENO);
        unless(last) redirect(input(current) to STDOUT_FILENO);
      }

      previous[0] = output(current);
      previous[1] = current[1];
    }
    wait_all;
  }
}
