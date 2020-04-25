#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#include <assert.h>
#include <wait.h>
#include <errno.h>
#include <string.h>

enum package_status {
  unused = 0,
  recived = 1,
  processed = 2,
  sent = 0,
};

#define MAX_PACKAGES 100
#define PERMISSIONS 0600
typedef struct package {
  enum package_status status;
  int size;
} package;

struct shared {
  int unused, recived, processed;
  struct package packages[MAX_PACKAGES];
};

struct sembuf sem_wait = {1, 0, SEM_UNDO};
struct sembuf sem_inc = {1, 1, SEM_UNDO};
struct sembuf sem_dec = {1, -1, SEM_UNDO};
#define using(semid)                                                           \
  for (int x = 1; x && (semop(semid, &sem_dec, 1), 1);                         \
       x = (semop(semid, &sem_inc, 1), 0))
#define repeat for (;;)

time_t timer;
char buffer[26];
struct tm *tm_info;

#define log(x, args...)                                                        \
  {                                                                            \
    timer = time(NULL);                                                        \
    tm_info = localtime(&timer);                                               \
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);                        \
    printf("(%d %s) " x "\n", getpid(), buffer, args);                         \
  }

#define worker int main(int argc, char **argv)