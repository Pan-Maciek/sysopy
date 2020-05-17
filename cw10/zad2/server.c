#include "common.h"
#include "message.h"
#include <pthread.h>

#define MAX_CONN 16
#define PING_INTERVAL 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

union addr {
  struct sockaddr_un uni;
  struct sockaddr_in web;
};
typedef struct sockaddr* sa;

struct client {
  union addr addr;
  int sock, addrlen;
  enum client_state { empty = 0, waiting, playing } state;
  struct client* peer;
  char nickname[16];
  char symbol;
  struct game_state* game_state;
  bool responding;
} clients[MAX_CONN], *waiting_client = NULL;
typedef struct client client;

void delete_client(client* client) {
  printf("Deleting %s\n", client->nickname);
  if (client == waiting_client) waiting_client = NULL;
  if (client->peer) {
    client->peer->peer = NULL;
    client->peer->game_state = NULL;
    delete_client(client->peer);
    free(client->game_state);
    client->peer = NULL;
    client->game_state = NULL;
  }
  message msg = { .type = msg_disconnect };
  sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
  memset(&client->addr, 0, sizeof client->addr);
  client->state = empty;
  client->sock = 0;
  client->nickname[0] = 0;
}

void send_gamestate(client* client) {
  message msg = { .type = msg_state };
  memcpy(&msg.payload.state, client->game_state, sizeof (struct game_state));
  sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
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

bool check_draw(client* client) {
  for (int i = 0; i < 9; i++)
    if (client->game_state->board[i] == '-')
      return false;
  return true;
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
  sendto(client1->sock, &msg, sizeof msg, 0, (sa) &client1->addr, client1->addrlen);

  strncpy(msg.payload.play.nickname, client1->nickname, sizeof client1->nickname);
  client2->symbol = msg.payload.play.symbol = 'o';
  sendto(client2->sock, &msg, sizeof msg, 0, (sa) &client2->addr, client2->addrlen);

  client1->game_state->move = client1->symbol;
  send_gamestate(client1);
  send_gamestate(client2);
}

void on_client_message(client* client, message* msg) {
  if (msg->type == msg_ping) {
    pthread_mutex_lock(&mutex);
    printf("pong %s\n", client->nickname);
    client->responding = true;
    pthread_mutex_unlock(&mutex);
  }
  else if (msg->type == msg_disconnect) {
    pthread_mutex_lock(&mutex);
    delete_client(client);
    pthread_mutex_unlock(&mutex);
  } else if (msg->type == msg_move) {
    int move = msg->payload.move;
    if (msg->type != msg_move) return;
    if (client->game_state->move == client->symbol 
      && client->game_state->board[move] == '-'
      && 0 <= move && move <= 8) {
      client->game_state->board[move] = client->symbol;
      client->game_state->move = client->peer->symbol;
      
        printf("%s moved\n", client->nickname);
        send_gamestate(client);
        send_gamestate(client->peer);
      if (check_game(client)) {
        msg->type = msg_win;
        msg->payload.win = client->symbol;
      }
      else if (check_draw(client)) {
        msg->type = msg_win;
        msg->payload.win = '-';
      }
      if (msg->type == msg_win) {
        printf("Game between %s and %s finised\n", client->nickname, client->peer->nickname);
        client->peer->peer = NULL;
        sendto(client->peer->sock, msg, sizeof msg, 0, (sa) &client->peer->addr, client->peer->addrlen);
        sendto(client->sock, msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
        delete_client(client);
      } 
    } 
    else send_gamestate(client);
  }
}

void init_socket(int socket, void* addr, int addr_size) {
  safe (bind(socket, (struct sockaddr*) addr, addr_size));
  struct epoll_event event = { 
    .events = EPOLLIN | EPOLLPRI, 
    .data = { .fd = socket } 
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

void new_client(union addr* addr, socklen_t addrlen, int sock, char* nickname) {
  pthread_mutex_lock(&mutex);
  int empty_index = -1;
  for (int i = 0; i < MAX_CONN; i++) {
    if (clients[i].state == empty) empty_index = i;
    else if (strncmp(nickname, clients[i].nickname, sizeof clients->nickname) == 0) {
      pthread_mutex_unlock(&mutex);
      message msg = {.type = msg_username_taken };
      printf("Nickname %s already taken\n", nickname);
      sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
      return;
    }
  }
  if (empty_index == -1) {
    pthread_mutex_unlock(&mutex);
    printf("Server is full\n");
    message msg = { .type = msg_server_full };
    sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
    return;
  }
  printf("New client %s\n", nickname);
  client* client = &clients[empty_index];
  memcpy(&client->addr, addr, addrlen);
  client->addrlen = addrlen;
  client->state = waiting;
  client->responding = true;
  client->sock = sock;

  memset(client->nickname, 0, sizeof client->nickname);
  strncpy(client->nickname, nickname, sizeof client->nickname - 1);
  if (waiting_client) {
    printf("Connecting %s with %s\n", client->nickname, waiting_client->nickname);
    if (rand() % 2 == 0) join_clients(client, waiting_client);
    else join_clients(waiting_client, client);
    waiting_client = NULL;
  } else {
    printf("%s is waiting\n", client->nickname);
    message msg = { .type = msg_wait };
    sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
    waiting_client = client;
  }

  pthread_mutex_unlock(&mutex);
}

void* ping(void* _) {
  const static message msg = { .type = msg_ping };
  loop {
    sleep(PING_INTERVAL);
    pthread_mutex_lock(&mutex);
    printf("Pinging clients\n");
    for (int i = 0; i < MAX_CONN; i++) {
      if (clients[i].state != empty) {
        if (clients[i].responding) {
          clients[i].responding = false;
          sendto(clients[i].sock, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addrlen);
        }
        else delete_client(&clients[i]);
      }
    }
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    print("Usage [port] [path]\n");
    exit(0);
  }
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
  int local_sock = safe (socket(AF_UNIX, SOCK_DGRAM, 0));
  init_socket(local_sock, &local_addr, sizeof local_addr);

  int web_sock = safe (socket(AF_INET, SOCK_DGRAM, 0));
  init_socket(web_sock, &web_addr, sizeof web_addr);

  pthread_t ping_thread;
  pthread_create(&ping_thread, NULL, ping, NULL);

  struct epoll_event events[10];
  loop {
    int nread = safe (epoll_wait(epoll_fd, events, 10, -1));
    repeat (nread) {
      int sock = events[i].data.fd;
      message msg;
      union addr addr;
      socklen_t addrlen = sizeof addr;
      recvfrom(sock, &msg, sizeof msg, 0, (sa) &addr, &addrlen);
      if (msg.type == msg_connect) {
        new_client(&addr, addrlen, sock, msg.payload.nickname);
      } else {
        int i = find(int i = 0; i < MAX_CONN; i++, memcmp(&clients[i].addr, &addr, addrlen) == 0);
        on_client_message(&clients[i], &msg);
      }
    }
  }
}