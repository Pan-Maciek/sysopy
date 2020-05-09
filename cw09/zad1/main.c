#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_CLIENT_CREATION_OFFSET 3
#define MAX_BARBER_ACTION_TIME 5
#define MAX_CLIENT_RETRY_DELAY 2

#define repeat(n) for (int i = 0, __size = n; i < __size; i++)
#define loop for(;;)
#define using(mutex) for (int __run = 1; __run && (pthread_mutex_lock(&mutex), 1) ; __run = (pthread_mutex_unlock(&mutex), 0))
#define find(init, cond) ({ int index = -1;  for (init) if (cond) { index = i; break; } index; })
#define copy(arg) ({ typeof(arg)* copy = malloc(sizeof *copy); *copy = arg; copy; })
#define p_creat(thread_ptr, fn, arg) pthread_create(thread_ptr, NULL, (void* (*)(void*)) fn, copy(arg))

#define LOG1 "\033[47;30m"
#define LOG2 "\033[100;97m"
#define DEF  "\033[0m"
#define ENDL "\r\n"

pthread_mutex_t chair_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_cond = PTHREAD_COND_INITIALIZER;

int client_count, chair_count, free_chairs;
id_t* chairs; 
bool barber_sleeping = false;

void* barber(void* _) {
  repeat (client_count) {
    using(chair_mutex) {
      while (free_chairs == chair_count) {
        printf(LOG1"  Golibroda  "DEF" Idę spać"ENDL);
        barber_sleeping = true;
        pthread_cond_wait(&barber_cond, &chair_mutex);
      }
      barber_sleeping = false;
      int client_index = find(int i = 0; i < client_count; i++, chairs[i] != 0);

      free_chairs++;
      printf(LOG1"  Golibroda  "DEF" Czeka %d klientow, golę klienta %u"ENDL, chair_count - free_chairs, chairs[client_index]);
      chairs[client_index] = 0;
    }

    sleep(rand() % MAX_BARBER_ACTION_TIME + 1);
  }
  printf(LOG1"  Golibroda  "DEF" Koniec pracy"ENDL);
  return NULL;
}

void* client(id_t* id) {
  bool done = false;
  loop {
    using(chair_mutex) {
      if (free_chairs == 0) {
        printf(LOG1"   Klient    "LOG2"  %u  "DEF" Zajęte"ENDL, *id);
        continue;
      }

      int free_chair_index = find(int i = 0; i < chair_count; i++, chairs[i] == 0);
      chairs[free_chair_index] = *id;

      printf(LOG1"   Klient    "LOG2"  %u  "DEF" Poczekalnia, wolne miejsca %d"ENDL, *id, free_chairs);
      if (free_chairs == chair_count && barber_sleeping) {
        printf(LOG1"   Klient    "LOG2"  %u  "DEF" Budze golibrodę"ENDL, *id);
        pthread_cond_broadcast(&barber_cond);
      }
      free_chairs--;
      done = true;
    }

    if (done) return free(id), NULL;
    sleep(rand() % MAX_CLIENT_RETRY_DELAY + 1);
  }
}

int main(int argc, char** argv) {
  assert(argc == 3);

  client_count = atoi(argv[1]);
  assert(client_count > 0);

  free_chairs = chair_count = atoi(argv[2]);
  assert(chair_count > 0);

  srand(time(NULL));

  chairs = calloc(chair_count, sizeof *chairs);

  pthread_t barber_thread, *client_thread = malloc(client_count * sizeof  *client_thread);
  pthread_create(&barber_thread, NULL, barber, NULL);

  repeat (client_count) {
    p_creat(&client_thread[i], client, i + 1);
    sleep(rand() % MAX_CLIENT_CREATION_OFFSET + 1);
  }

  repeat (client_count) pthread_join(client_thread[i], NULL);
  pthread_join(barber_thread, NULL);
}