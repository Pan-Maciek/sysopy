#define CLIENT
#include "events.c"
#include <fcntl.h>

def init { // this code runs at the start of the program
  key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(server_key != -1);

  server_qid = msgget(server_key, QUEUE_PERMISSIONS);
  assert(server_qid != -1);

  client_qid = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
  assert(client_qid != -1);

  sendi(Init, server_qid, { client_qid });
  recive (client_qid) 
    switch(msg.mtype) {
      handle(Init)
      case Error:
        print(left(100) "Init: " clearline() red "ERROR " cdefault);
        on_Error(&msg.payload.response.Error);
        exit(0);
    }
  assert(client_id != 0);
  assert(fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK) != -1);
}

def finish { // this code runs atexit and on SIGINT
  sendi(Stop, server_qid, { client_qid });
  msgctl(client_qid, IPC_RMID, NULL);
}

def event_loop {
  char action_str[10];

  repeat {
    if (scanf("%s %99[^\n]", action_str, msg.payload.response.Message) > 0) {
      switch(parse_msg_t(action_str)) {
        case List: sendi(List, server_qid, { client_qid }); break;
        case Stop: sendi(Stop, server_qid, { client_qid }); break;
        case Disconnect: sendi(Disconnect, server_qid, { client_qid }); break;
        case Connect: 
          peer_id = atoi(msg.payload.response.Message);
          sendi(Connect, server_qid, { client_qid, peer_id });
        break;
        case Message:
        if (peer_qid) {
          msg.mtype = Message;
          msgsnd(peer_qid, &msg, MAX_MESSAGE, 0);
        } else print(red "Not connected\n" cdefault);
        break;
        default:
          print(red "Invalid action\n" cdefault);
        break;
      }
    }
    recive_async (client_qid) {
      switch(msg.mtype) {
        handle(Stop)
        handle(Disconnect)
        handle(List)
        handle(Connect)
        handle(Error)
        handle(Message)
      }
    }
    usleep(250);
  }
}