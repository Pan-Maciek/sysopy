#include <stdlib.h>

#include "events.c"

qid_t server_qid;

def init { // this code runs at the start of the program
  key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(server_key != -1);

  server_qid = msgget(server_key, IPC_CREAT | IPC_EXCL | QUEUE_PERMISSIONS);
  assert(server_qid != -1);
}

def finish { // this code runs atexit and on SIGINT
  foreach(c, clients, 1, MAX_CLIENTS)
    if (c->qid) send0(Stop, c->qid);
  msgctl(server_qid, IPC_RMID, NULL);
}

def event_loop { // this code is run after init is done
  repeat recive (server_qid) {
    switch(msg.mtype) {
      handle(Init)
      handle(Connect)
      handle(Disconnect)
      handle(List)
      handle(Stop)
    }
  }
}
