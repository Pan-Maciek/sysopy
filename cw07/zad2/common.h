#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <assert.h>
#include <wait.h>

enum package_status {
  unused = 0,
  recived = 1,
  processed = 2,
};

#define MAX_PACKAGES 10
#define PERMISSIONS 0600
typedef struct package {
  enum package_status status;
  int size;
} package;

struct shared {
  int unused, recived, processed;
  struct package packages[MAX_PACKAGES];
} * shared;

#define using(semid)                                                           \
  for (int x = 1; x && (sem_wait(sem), 1); x = (sem_post(sem), 0))
#define loop for (;;)
#define repeat(n) for (int i = 0; i < n; i++)