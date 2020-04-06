#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_SIZE "10"
#define PRODUCER  "./producer.out"
#define CONSUMENT "./customer.out"
#define FIFO      "test.fifo"
#define CONFIRM   "conf.txt"
#define TESTS     5

#define wait_all while (wait(NULL) > 0) ;
#define do if (fork() == 0)
#define exec(exe, args...) execlp(exe, exe, args, NULL)
#define repeat(n) for (int i = 0; i < n; i++)
#define then

char* itoa(int value) {
  static char buff[12];
  sprintf(buff, "%i", value);
  return strdup(buff);
}

char* generate(char* base, int id) {
  char* filename = calloc(strlen(base) + 12, sizeof(char));
  sprintf(filename, "%s_%i.txt", base, id);
  FILE* file = fopen(filename, "w+");
  repeat (25) fprintf(file, "%i", id);
  fclose(file);
  return filename;
}

int main() {
  if (mkfifo(FIFO, 0644) != 0)
    panic("Can not create fifo '%s': %s", FIFO, strerror(errno));

  do {
    exec(CONSUMENT, FIFO, CONFIRM, READ_SIZE);
    panic("Can not exec '%s': %s", CONSUMENT, strerror(errno));
  }

  repeat (TESTS) {
    char* size = itoa(rand() % 10 + 5);
    do {
      char* test_file = generate("test", i);
      exec(PRODUCER, FIFO, test_file, size);
      panic("Can not exec '%s': %s", PRODUCER, strerror(errno));
    }
  }

  wait_all then remove(FIFO);
}
