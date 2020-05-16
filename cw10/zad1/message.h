struct game_state { char move; char board[9]; };

typedef struct message {
  enum message_type { 
    msg_play,
    msg_move,
    msg_state,
    msg_win,
    msg_ping
  } type;
  union message_payload {
    struct { char nickname[16]; char symbol; } play;
    int move;
    struct game_state state;
    char win;
  } payload;
} message;
