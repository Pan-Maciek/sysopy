#include "worker.h"

worker {
  loop using(semid) {
    if (shared->processed == 0) continue;
    package *package = find(shared, status == processed);
    update(shared, package, processed, unused);
    package->size *= 3;
    log("Wysłąłem zamówienie o wielkości: %d. Liczba zamówień do "
        "przygotowania: %d, Liczba zamówień do wyslania: %d",
        package->size, shared->recived, shared->processed);
  }
}