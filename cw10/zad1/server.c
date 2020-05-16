#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

#include <sys/epoll.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "message.h"

#define safe(expr)                                                              \
  ({                                                                            \
    typeof(expr) __tmp = expr;                                                  \
    if (__tmp == -1) {                                                          \
      printf("%s:%d "#expr" failed: %s\n", __FILE__, __LINE__, strerror(errno));\
      exit(EXIT_FAILURE);                                                       \
    }                                                                           \
    __tmp;                                                                      \
  })
#define loop for(;;)
#define find(init, cond) ({ int index = -1;  for (init) if (cond) { index = i; break; } index; })
#define repeat(n) for(int i = 0; i < n; i++)

#define MAX_CONN 16


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

typedef struct client client;

struct client {
  int fd;
  enum client_state { empty = 0, init, waiting, playing } state;
  struct client* peer;
  char nickname[16];
  char symbol;
  struct game_state* game_state;
} clients[MAX_CONN];
client* waiting_client = NULL;


typedef struct event_data {
  enum event_type { socket_event, client_event } type;
  union payload { client* client; int socket; } payload;
} event_data;

void delete_client(client* client) {
  if (client == waiting_client) waiting_client = NULL;
  if (client->peer) {
    client->peer->peer = NULL;
    client->peer->game_state = NULL;
    delete_client(client->peer);
    free(client->game_state);
    client->peer = NULL;
    client->game_state = NULL;
  }
  client->state = empty;
  client->nickname[0] = 0;
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
  close(client->fd);
}

void send_gamestate(client* client) {
  message msg = { .type = msg_state };
  memcpy(&msg.payload.state, client->game_state, sizeof (struct game_state));
  write(client->fd, &msg, sizeof msg);
}

int cell_coord(int x, int y) {
  return x + y * 3;
}


bool check_game(client* client) {
  static int win[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6},
    {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}
  };
  char* board = client->game_state->board;
  char c = client->symbol;
  for (int i = 0; i < 8; i++) {
    if (board[win[i][0]] == c &&
        board[win[i][1]] == c && 
        board[win[i][2]] == c) 
        return true;
  }
  return false;
}

void join_clients(client* client1, client* client2) {
  client1->state = client2->state = playing;
  client1->peer = client2;
  client2->peer = client1;

  client1->game_state = client2->game_state = calloc(1, sizeof client1->game_state);
  memset(client1->game_state->board, '-', 9);

  message msg = { .type = msg_play };
  strncpy(msg.payload.play.nickname, client2->nickname, sizeof client2->nickname);
  client1->symbol = msg.payload.play.symbol = 'x';
  write(client1->fd, &msg, sizeof msg);

  strncpy(msg.payload.play.nickname, client1->nickname, sizeof client1->nickname);
  client2->symbol = msg.payload.play.symbol = 'o';
  write(client2->fd, &msg, sizeof msg);

  client1->game_state->move = client1->symbol;
  send_gamestate(client1);
  send_gamestate(client2);
}

void on_client_message(client* client) {
  if (client->state == init) {
    int nick_size = read(client->fd, client->nickname, sizeof client->nickname - 1);
    int j = client - clients;
    pthread_mutex_lock(&mutex);
    if (find(int i = 0; i < MAX_CONN; i++, i != j && strncmp(client->nickname, clients[i].nickname, sizeof clients->nickname) == 0) == -1) {
      client->nickname[nick_size] = '\0';
      if (waiting_client) {
        if (rand() % 2 == 0) join_clients(client, waiting_client);
        else join_clients(waiting_client, client);
        waiting_client = NULL;
      } else {
        waiting_client = client;
        client->state = waiting;
      }
    } 
    else delete_client(client); // username taken
    pthread_mutex_unlock(&mutex);
  }
  else if (client->state == playing) {
    message msg;
    read(client->fd, &msg, sizeof msg);
    int move = msg.payload.move;
    if (client->game_state->move == client->symbol 
      && client->game_state->board[move] == '-'
      && 0 <= move && move <= 8) {
      client->game_state->board[move] = client->symbol;
      client->game_state->move = client->peer->symbol;
      
      send_gamestate(client);
      send_gamestate(client->peer);
      if (check_game(client)) {
        msg.type = msg_win;
        msg.payload.win = client->symbol;
        write(client->fd, &msg, sizeof msg);
        write(client->peer->fd, &msg, sizeof msg);
      }
    } 
    else send_gamestate(client);
  }
}

void init_socket(int socket, void* addr, int addr_size) {
  safe (bind(socket, (struct sockaddr*) addr, addr_size));
  safe (listen(socket, MAX_CONN));
  struct epoll_event event = { .events = EPOLLIN | EPOLLPRI };
  event_data* event_data = event.data.ptr = malloc(sizeof *event_data);
  event_data->type = socket_event;
  event_data->payload.socket = socket;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

client* new_client(int client_fd) {
  pthread_mutex_lock(&mutex);
  int i = find(int i = 0; i < MAX_CONN; i++, clients[i].state == empty);
  if (i == -1) return NULL;
  client* client = &clients[i];

  client->fd = client_fd;
  client->state = init;
  pthread_mutex_unlock(&mutex);
  return client;
}

int main(int argc, char** argv) {
  int port = atoi(argv[1]);
  char* socket_path = argv[2];

  epoll_fd = safe (epoll_create1(0));

  struct sockaddr_un local_addr = { .sun_family = AF_UNIX };
  strncpy(local_addr.sun_path, socket_path, sizeof local_addr.sun_path);

  struct sockaddr_in web_addr = {
    .sin_family = AF_INET, .sin_port = htons(port),
    .sin_addr = { .s_addr = htonl(INADDR_ANY) },
  };

  unlink(socket_path);
  int local_sock = safe (socket(AF_UNIX, SOCK_STREAM, 0));
  init_socket(local_sock, &local_addr, sizeof local_addr);

  int web_sock = safe (socket(AF_INET, SOCK_STREAM, 0));
  init_socket(web_sock, &web_addr, sizeof web_addr);

  struct epoll_event events[10];
  loop {
    int nread = safe (epoll_wait(epoll_fd, events, 10, -1));
    repeat (nread) {
      event_data* data = events[i].data.ptr;
      if (data->type == socket_event) { // new conenction
        int client_fd = accept(data->payload.socket, NULL, NULL);
        client* client = new_client(client_fd);
        if (client == NULL) {
          close(client_fd); 
          continue;
        }

        event_data* event_data = malloc(sizeof(event_data));
        event_data->type = client_event;
        event_data->payload.client = client;
        struct epoll_event event = { .events = EPOLLIN | EPOLLET | EPOLLHUP, .data = { event_data } };

        safe (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event));
      } else if (data->type == client_event) {
        if (events[i].events & EPOLLHUP) {
          pthread_mutex_lock(&mutex);
          delete_client(data->payload.client);
          pthread_mutex_unlock(&mutex);
        }
        else on_client_message(data->payload.client);
      }
    }
  }
}