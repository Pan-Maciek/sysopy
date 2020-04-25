#include "common.h"

worker {
  key_t key = ftok("/home/maciek", 'B');
  int shmid = shmget(key, sizeof(struct shared), PERMISSIONS);
  assert(shmid != -1);

  struct shared *shared = shmat(shmid, NULL, 0);
  assert(shared != NULL);

  shared->unused = MAX_PACKAGES;

  int semid = semget(key, 0, PERMISSIONS);
  assert(semid != -1);

  repeat {
    using(semid) {
      if (shared->recived == 0)
        continue;
      package *package = NULL;
      for (int i = 0; i < MAX_PACKAGES; i++)
        if (shared->packages[i].status == recived) {
          package = shared->packages + i;
          break;
        }
      shared->recived -= 1;
      shared->processed += 1;
      package->status = processed;
      package->size *= 2;
      log("Przygotowałem zamówienie o wielkości: %d. Liczba zamówień do "
          "przygotowania: %d, Liczba "
          "zamówień do wyslania: %d",
          package->size, shared->recived, shared->processed);
    }
    sleep(1);
  }
}