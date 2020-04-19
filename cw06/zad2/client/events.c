#ifndef events_c
#define events_c

#include "logic.c"

on(Init) {
  assert(client_id == 0); // trying to reinitialize client_id is not permited
  client_id = payload->id;
  return Ok;
}

on(Stop) {
  server_qid = -1; // prevent request to the server at exit
  return exit(0), Ok;
}

on(Disconnect) {
  printf(cyan "Disconnected " cdefault "from %d\n", peer_id);
  return disconnect(), Ok;
}

on(List) {
  assert(payload->size > 0);
  print("id\tstate\n");
  foreach (c, payload->clients, 0, payload->size) {
    printf(cdefault "%u\t", c->id);
    if (c->id == peer_id) printf(green "peer\n");
    else if (c->id == client_id) printf(yellow "you\n");
    else if (c->available) printf(cdefault "available\n");
    else printf(red "not available\n");
  }
  print(cdefault);
  return Ok;
}

on(Connect) {
  peer_pid = payload->peer_pid;
  peer_qid = mq_open(path(peer_pid), O_WRONLY, QUEUE_PERMISSIONS, QUEUE_ATTR);
  peer_id = payload->peer_id;
  printf(cyan "Connected " cdefault "to %d\n", peer_id);
  return Ok;
}

on (Message) { // message from peer
  printf(cyan "Recived " cdefault "%s\n", *payload);
  return Ok;
}

on(Error) { // on server error
  switch (*payload) { 
    case Ok: break;
    case server_full: printf("Server is full.\n"); break;
    case bad_id: printf("Bad id.\n"); break;
    case bad_qid: printf("Bad qid.\n"); break;
  }
  return Ok;
}

#endif