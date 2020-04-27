#include "common.h"
#include <time.h>

long int now() {
  static struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

#define log(x, args...) printf("(%d %ld) " x "\n", getpid(), now(), args);

#define find(mem, x)                                                           \
  ({                                                                           \
    struct package *y = 0;                                                     \
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

#define worker void worker_fn()
void worker_fn();

int shmid, semid;
int main() {
  key_t key = ftok(HOME_PATH, PROJECT_ID);
  shmid = shmget(key, 0, PERMISSIONS);
  assert(shmid != -1);

  shared = shmat(shmid, NULL, 0);
  assert(shared != NULL);

  semid = semget(key, 0, PERMISSIONS);
  assert(semid != -1);

  worker_fn();
}
