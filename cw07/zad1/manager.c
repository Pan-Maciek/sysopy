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
void at_exit() {
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 10, IPC_RMID, NULL);
}

int main() {
  atexit(at_exit);
  signal(SIGINT, exit);

  key_t key = ftok(HOME_PATH, PROJECT_ID);
  shmid = shmget(key, sizeof(struct shared), IPC_CREAT | IPC_EXCL | PERMISSIONS);
  assert(shmid != -1);

  shared = shmat(shmid, NULL, 0);
  assert(shared != NULL);
  shared->unused = MAX_PACKAGES;

  semid = semget(key, 100, IPC_CREAT | IPC_EXCL | PERMISSIONS);
  assert(semid != -1);
  semop(semid, &sem_inc, 1);

  repeat (10) spawn("./worker1.out");
  repeat (10) spawn("./worker2.out");
  repeat (10) spawn("./worker3.out");
  
  while(wait(NULL) != -1);

  return 0;
}