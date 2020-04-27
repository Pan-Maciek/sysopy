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

int shmid;
sem_t *sem;
void at_exit() {
  munmap(shared, sizeof(struct shared));
  sem_close(sem);
}

int main() {
  atexit(at_exit);
  signal(SIGINT, exit);
  shmid = shm_open("/shared", O_RDWR, PERMISSIONS);
  assert(shmid != -1);

  shared = mmap(0, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
  assert(shared != NULL);

  sem = sem_open("/shared", 0);
  assert(sem != NULL);

  worker_fn();
}
