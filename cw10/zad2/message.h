struct game_state { char move; char board[9]; };

typedef struct message {
  enum message_type { 
    msg_connect, msg_disconnect,
    msg_play, msg_move,
    msg_state, msg_win,
    msg_ping, msg_username_taken,
    msg_server_full, msg_wait
  } type;
  union message_payload {
    char nickname[16];
    struct { char nickname[16]; char symbol; } play;
    int move;
    struct game_state state;
    char win;
  } payload;
} message;
