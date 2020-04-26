#include "worker.h"

worker {
  loop using(semid) {
    if (shared->recived == 0) continue;
    package *package = find(shared, status == recived);
    update(shared, package, recived, processed);
    package->size *= 2;
    log("Przygotowałem zamówienie o wielkości: %d. Liczba zamówień do "
        "przygotowania: %d, Liczba zamówień do wyslania: %d",
        package->size, shared->recived, shared->processed);
  }
}