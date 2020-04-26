#include "common.h"
#define worker void worker_fn()

void worker_fn();

int shmid;
sem_t* sem;
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
