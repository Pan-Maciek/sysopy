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
#define print(x) write(STDOUT_FILENO, x, sizeof(x))

int connect_unix(char* path) {
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof addr.sun_path);

  int sock = safe (socket(AF_UNIX, SOCK_STREAM, 0));
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
"\033[0;0H\033[J\033[90m\n\n"
"   1 │ 2 │ 3    you\033[0m (%s) %s\n\033[90m"
"  ───┼───┼───      \033[0m (%s) %s\n\033[90m"
"   4 │ 5 │ 6 \n"
"  ───┼───┼───\n"
"   7 │ 8 │ 9 \n"
"\033[0m\n";
void draw_initial_state() {
  dprintf(STDOUT_FILENO, board, 
    state.symbols[0], state.nicknames[0],
    state.symbols[1], state.nicknames[1]);
}

void cell(int x, int y, char* text) {
  dprintf(STDOUT_FILENO, "\033[s\033[%d;%dH%s\033[u", y * 2 + 3, x * 4 + 4, text);
}

void render_footer() {
  if (state.game.move != state.symbol) 
    print("\033[9;0H\033[J\r [ ] \033[33mWait for your move.\r\033[0m\r\033[2C");
  else print("\033[9;0H\033[J\r [ ]\033[J\r\033[2C");
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

int main(int argc, char** argv) {
  char* nickname = state.nicknames[0] = argv[1];

  int sock = connect_unix("/tmp/sock");

  write(sock, nickname, strlen(nickname));
  
  message msg;
  read(sock, &msg, sizeof msg);

  if (msg.type == msg_play) {
    state.nicknames[1] = msg.payload.play.nickname;
    state.symbols[1] = msg.payload.play.symbol == 'o' ? "\033[36mx\033[0m" : "\033[32mo\033[0m";
    state.symbol = msg.payload.play.symbol;
    state.symbols[0] = msg.payload.play.symbol == 'o' ? "\033[32mo\033[0m" : "\033[36mx\033[0m";
    state.score[0] = state.score[1] = 0;
    draw_initial_state();
  } else exit(0);

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
        scanf("%d", &c);
        render_footer();
        if (c < 1 || c > 9) continue;
        c -= 1;
        int x = c % 3, y = c / 3;

        if (state.game.board[c] == '-') {
          cell(x, y, state.symbols[0]);
          message msg = { .type = msg_move };
          msg.payload.move = c;
          write(sock, &msg, sizeof msg);
        }
      } else {
        if (events[i].events & EPOLLHUP) exit(0);
        message msg;
        read(sock, &msg, sizeof msg);
        if (msg.type == msg_ping) write(sock, &msg, sizeof msg);
        else if (msg.type == msg_state) {
          memcpy(&state.game, &msg.payload.state, sizeof state.game);
          render_update();
        } else if (msg.type == msg_win) {
          if (msg.payload.win == state.symbol) printf("\r You won ^^\33[J\n\n");
          else printf("\r You lost T_T\n\n");
          close(sock);
          exit(0);
        }
      }
    }
  }
}