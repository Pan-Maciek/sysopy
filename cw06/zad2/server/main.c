#include <stdlib.h>

#include "events.c"

mqd_t server_qid;

def init { // this code runs at the start of the program
  server_qid = mq_open(SERVER_PATH, O_CREAT | O_RDWR | O_EXCL, QUEUE_PERMISSIONS, QUEUE_ATTR);
  assert(server_qid != -1);
}

def finish { // this code runs atexit and on SIGINT
  foreach(c, clients, 1, MAX_CLIENTS)
    if (c->qid) {
      send0(Stop, c->qid);
      mq_close(c->qid);
    }
  mq_close(server_qid);
  mq_unlink(SERVER_PATH);
}

def event_loop { // this code is run after init is done
  repeat recive (server_qid) 
    switch(msg.mtype) {
      handle(Init)
      handle(Connect)
      handle(Disconnect)
      handle(List)
      handle(Stop)
    }
}
