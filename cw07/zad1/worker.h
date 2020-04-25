#include "common.h"
#define worker void worker_fn()

void worker_fn();

int shmid, semid;
int main() {
  key_t key = ftok("/home/maciek", 'B');
  shmid = shmget(key, 0, PERMISSIONS);
  assert(shmid != -1);

  shared = shmat(shmid, NULL, 0);
  assert(shared != NULL);

  semid = semget(key, 0, PERMISSIONS);
  assert(semid != -1);

  worker_fn();
}
