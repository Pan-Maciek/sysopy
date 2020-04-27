#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pwd.h>

#include <assert.h>
#include <wait.h>

enum package_status {
  unused = 0,
  recived = 1,
  processed = 2,
};

#define MAX_PACKAGES 10
#define PERMISSIONS 0600
#define HOME_PATH (getpwuid(getuid())->pw_dir)
#define PROJECT_ID 'M'

typedef struct package {
  enum package_status status;
  int size;
} package;

struct shared {
  int unused, recived, processed;
  struct package packages[MAX_PACKAGES];
} *shared;

struct sembuf sem_wait = {1, 0, SEM_UNDO};
struct sembuf sem_inc = {1, 1, SEM_UNDO};
struct sembuf sem_dec = {1, -1, SEM_UNDO};
#define using(semid)                                                           \
  for (int x = 1; x && (semop(semid, &sem_dec, 1), 1);                         \
       x = (semop(semid, &sem_inc, 1), 0))
#define loop for (;;)
#define repeat(n) for (int i = 0; i < n; i++)