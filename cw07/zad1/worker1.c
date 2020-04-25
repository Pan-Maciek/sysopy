#include "worker.h"

worker {
  loop using(semid) {
    if (shared->unused == 0) continue;
    package *package = find(shared, status == unused);
    update(shared, package, unused, recived);
    package->size = rand() % 10;
    log("Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d, Liczba "
        "zamówień do wyslania: %d",
        package->size, shared->recived, shared->processed);
  }
}