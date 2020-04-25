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
      if (shared->unused == 0)
        continue;
      int size = rand() % 10;
      package *package = NULL;
      for (int i = 0; i < MAX_PACKAGES; i++)
        if (shared->packages[i].status == unused) {
          package = shared->packages + i;
          break;
        }
      shared->unused -= 1;
      shared->recived += 1;
      package->status = recived;
      package->size = size;
      log("Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d, Liczba "
          "zamówień do wyslania: %d",
          size, shared->recived, shared->processed);
    }
    sleep(1);
  }
}