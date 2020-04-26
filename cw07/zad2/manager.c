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

int shmid;
sem_t* sem;
void at_exit() {
  signal(SIGINT, SIG_IGN);
  kill(0, SIGINT);
  munmap(shared, sizeof(struct shared));
  shm_unlink("/shared");
  sem_close(sem);
  sem_unlink("/shared");
}

int main() {
  atexit(at_exit);
  signal(SIGINT, exit);

  shmid = shm_open("/shared", O_CREAT | O_RDWR | O_EXCL, PERMISSIONS);
  ftruncate(shmid, sizeof(struct shared));
  assert(shmid != -1);

  shared = mmap(0, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
  assert(shared != NULL);
  shared->unused = MAX_PACKAGES;

  sem = sem_open("/shared", O_CREAT | O_EXCL, PERMISSIONS, 1);
  assert(sem != NULL);

  repeat (10) spawn("./worker1.out");
  repeat (10) spawn("./worker2.out");
  repeat (10) spawn("./worker3.out");

  while(wait(NULL) != -1);

  return 0;
}