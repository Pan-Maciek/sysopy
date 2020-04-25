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

  loop {
    using(semid) {
      if (shared->unused == 0)
        continue;
      package *package = find(shared, status == unused);
      update(shared, package, unused, recived);
      package->size = rand() % 10;
      log("Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d, Liczba "
          "zamówień do wyslania: %d",
          package->size, shared->recived, shared->processed);
    }
    sleep(1);
  }
}