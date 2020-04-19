#define CLIENT
#include "events.c"
#include <fcntl.h>
#include <limits.h>

char client_path[15];
def init { // this code runs at the start of the program
  server_qid = mq_open(SERVER_PATH, O_RDWR, QUEUE_PERMISSIONS, NULL);
  assert(server_qid != -1);

  pid_t pid = getpid();
  sprintf(client_path, "/c%d", pid);
  client_qid = mq_open(client_path, O_CREAT | O_RDWR | O_EXCL, QUEUE_PERMISSIONS, QUEUE_ATTR);
  assert(client_qid != -1);

  sendi(Init, server_qid, { pid });
  recive (client_qid) switch(msg.mtype) {
    handle(Init);
    case Error:
      exit(0);
  }
  assert(client_id != 0);
  assert(fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK) != -1);
}

def finish { // this code runs atexit and on SIGINT
  send0(Stop, server_qid);
  mq_close(server_qid);

  mq_close(client_qid);
  mq_unlink(client_path);
}

def event_loop {
  struct mq_attr async, old;
  async.mq_flags = O_NONBLOCK;
  mq_getattr(client_qid, &old);
  mq_setattr(client_qid, &async, &old);
  char action_str[10];

  repeat {
    if (scanf("%s %99[^\n]", action_str, msg.payload.response.Message) > 0) {
      switch(parse_msg_t(action_str)) {
        case List: sendi(List, server_qid, { client_id }); break;
        case Stop: sendi(Stop, server_qid, { client_id }); break;
        case Disconnect: sendi(Disconnect, server_qid, { client_id }); break;
        case Connect: 
          peer_id = atoi(msg.payload.response.Message);
          sendi(Connect, server_qid, { client_id, peer_id });
        break;
        case Message:
        if (peer_qid) {
          msg.mtype = Message;
          mq_send(peer_qid, (char*) &msg, MAX_MESSAGE, 0);
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