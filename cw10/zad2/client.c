#include "common.h"
#include "message.h"
#include <time.h>

int connect_unix(char* path, char* user) {
  struct sockaddr_un addr, bind_addr;
  memset(&addr, 0, sizeof(addr));
  bind_addr.sun_family = AF_UNIX;
  addr.sun_family = AF_UNIX;
  snprintf(bind_addr.sun_path, sizeof bind_addr.sun_path, "/tmp/%s%ld", user, time(NULL));
  strncpy(addr.sun_path, path, sizeof addr.sun_path);

  int sock = safe (socket(AF_UNIX, SOCK_DGRAM, 0));	
  safe (bind(sock, (void*) &bind_addr, sizeof addr));
  safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));

  return sock;
}

int connect_web(char* ipv4, int port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
    print("Invalid address\n");
    exit(0);
  }

  int sock = safe (socket(AF_INET, SOCK_DGRAM, 0));
  safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));
   
  return sock;
}

struct state {
  int   score[2];
  char* nicknames[2];
  char* symbols[2];
  char symbol;
  struct game_state game;
} state;

static const char board[] = 
"\033[0;0H\033[J\033[90m\n"
"   1 │ 2 │ 3    you\033[0m (%s) %s\n\033[90m"
"  ───┼───┼───      \033[0m (%s) %s\n\033[90m"
"   4 │ 5 │ 6 \n"
"  ───┼───┼───\n"
"   7 │ 8 │ 9 \n\n\033[0m";
void draw_initial_state() {
  dprintf(STDOUT_FILENO, board, 
    state.symbols[0], state.nicknames[0],
    state.symbols[1], state.nicknames[1]);
}

void cell(int x, int y, char* text) {
  dprintf(STDOUT_FILENO, "\033[s\033[%d;%dH%s\033[u", y * 2 + 2, x * 4 + 4, text);
}

void render_footer() {
  if (state.game.move != state.symbol) 
    print("\033[8;0H\033[J\r [ ] \033[33mWait for your move.\r\033[0m\r\033[2C");
  else print("\033[8;0H\033[J\r [ ]\033[J\r\033[2C");
}

void render_update() {
  static char tmp[12];
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++) {
      int c = x + y * 3;
      if (state.game.board[c] == '-') {
        sprintf(tmp, "\033[90m%d\033[0m", c + 1);
        cell(x, y, tmp);
      } 
      else cell(x, y, state.symbols[state.game.board[x + y * 3] == state.symbol ? 0 : 1]);
    }
  render_footer();
}

int sock;
void on_SIGINT(int _) {
  message msg = {.type = msg_disconnect};
  send(sock, &msg, sizeof msg, 0);
  exit(0);
}

int main(int argc, char** argv) {
  char* nickname;
  if (argc > 2) nickname = state.nicknames[0] = argv[1];
  if (argc == 5 && strcmp(argv[2], "web") == 0) sock = connect_web(argv[3], atoi(argv[4]));
  else if (argc == 4 && strcmp(argv[2], "unix") == 0) sock = connect_unix(argv[3], nickname);
  else {
    print("Usage [nick] [web|unix] [ip port|path]\n");
    exit(0);
  }
  
  signal(SIGINT, on_SIGINT);

  message msg = { .type = msg_connect };
  strncpy(msg.payload.nickname, nickname, sizeof msg.payload.nickname);
  send(sock, &msg, sizeof msg, 0);

  int epoll_fd = safe (epoll_create1(0));
  
  struct epoll_event stdin_event = { 
    .events = EPOLLIN | EPOLLPRI,
    .data = { .fd = STDIN_FILENO }
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

  struct epoll_event socket_event = { 
    .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
    .data = { .fd = sock }
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &socket_event);

  int c;
  struct epoll_event events[2];
  loop {
    int nread = safe (epoll_wait(epoll_fd, events, 2, 1));
    repeat(nread) {
      if (events[i].data.fd == STDIN_FILENO) {
        if (scanf("%d", &c) != 1) {
          char x;
          while ((x = getchar()) != EOF && x != '\n'); // skip invalid input
          render_footer();
          continue;
        }
        render_footer();
        if (c < 1 || c > 9) continue;
        c -= 1;
        int x = c % 3, y = c / 3;

        if (state.game.board[c] == '-') {
          cell(x, y, state.symbols[0]);
          message msg = { .type = msg_move };
          msg.payload.move = c;
          sendto(sock, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
        }
      } else {
        message msg;
        recvfrom(sock, &msg, sizeof msg, 0, NULL, NULL);
        if (msg.type == msg_wait) {
          print("Waiting for an opponent\n");
        } else if (msg.type == msg_play) {
          state.nicknames[1] = msg.payload.play.nickname;
          state.symbols[1] = msg.payload.play.symbol == 'o' ? "\033[36mx\033[0m" : "\033[32mo\033[0m";
          state.symbol = msg.payload.play.symbol;
          state.symbols[0] = msg.payload.play.symbol == 'o' ? "\033[32mo\033[0m" : "\033[36mx\033[0m";
          state.score[0] = state.score[1] = 0;
          draw_initial_state();
        } else if (msg.type == msg_username_taken) {
          print("This username is already taken\n");
          close(sock);
          exit(0);
        } else if (msg.type == msg_disconnect) {
          exit(0);
        } else if (msg.type == msg_server_full) {
          print("Server is full\n");
          close(sock);
          exit(0);
        } else if (events[i].events & EPOLLHUP) {
          print("\033[8;0H\033[J\r Disconnected X_x\n\n");
          exit(0);
        } else if (msg.type == msg_ping) 
          write(sock, &msg, sizeof msg);
        else if (msg.type == msg_state) {
          memcpy(&state.game, &msg.payload.state, sizeof state.game);
          render_update();
        } else if (msg.type == msg_win) {
          if (msg.payload.win == state.symbol) print("\033[8;0H\033[J\r You won ^.^\33[J\n\n");
          else if (msg.payload.win == '-') print("\033[8;0H\033[J\r It's a draw O_o\33[J\n\n");
          else print("\033[8;0H\033[J\r You lost T_T\n\n");
          exit(0);
        }
      }
    }
  }
}