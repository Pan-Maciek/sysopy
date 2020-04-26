#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <wait.h>

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
} * shared;

#define using(semid)                                                           \
  for (int x = 1; x && (sem_wait(sem), 1);                      \
       x = (sem_post(sem), 0))
#define loop for (;;)
#define repeat(n) for (int i = 0; i < n; i++)
#define find(mem, x)                                                           \
  ({                                                                           \
    struct package *y;                                                         \
    for (int i = 0; i < MAX_PACKAGES; i++)                                     \
      if (mem->packages[i].x) {                                                \
        y = mem->packages + i;                                                 \
        break;                                                                 \
      }                                                                        \
    y;                                                                         \
  })
#define update(shared, package, from, to)                                      \
  {                                                                            \
    shared->from -= 1;                                                         \
    shared->to += 1;                                                           \
    package->status = to;                                                      \
  }

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
