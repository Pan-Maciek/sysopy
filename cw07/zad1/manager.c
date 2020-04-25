#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "common.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <wait.h>

#define spawn(exe) if (fork() == 0 && execlp(exe, exe, NULL) == -1) exit(1);

int semid, shmid;
void on_SIGINT(int _) {
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 10, IPC_RMID, NULL);
}

void o() {
  on_SIGINT(0);
}

int main() {
  atexit(o);
  signal(SIGINT, on_SIGINT);

  key_t key = ftok("/home/maciek", 'B');
  shmid = shmget(key, sizeof(struct shared), IPC_CREAT | IPC_EXCL | PERMISSIONS);
  assert(shmid != -1);

  struct shared* shared = shmat(shmid, NULL, 0);
  assert(shared != NULL);
  shared->unused = MAX_PACKAGES;

  semid = semget(key, 100, IPC_CREAT | IPC_EXCL | PERMISSIONS);
  assert(semid != -1);
  semop(semid, &sem_inc, 1);

  for (int i = 0; i < 100; i++) {
    spawn("./worker1.out");
    spawn("./worker2.out");
    spawn("./worker3.out");
  }
  
  while(wait(NULL) != -1);

  return 0;
}