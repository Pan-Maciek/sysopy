#ifndef events_c
#define events_c

#include "logic.c"

on(Init) {
  client* client = new_client(payload->id);
  return client ? sendi(Init, client->qid, { client->id }) : server_full;
}

on(Stop) {
  return remove_client(payload->id), Ok;
}

on(Disconnect)  {
  return disconnect(payload->id), Ok;
}

on(List) {
  message msg = { List };
  int size = 0;

  struct client_info* info;
  mqd_t qid;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].id == payload->id) qid = clients[i].qid;
    if (clients[i].qid) {
      info = msg.payload.response.List.clients + size;
      info->id = clients[i].id;
      info->available = clients[i].peer == NULL;
      size++;
    }
  }
  
  msg.payload.response.List.size = size;
  mq_send(qid, (char*) &msg, (sizeof *info) * size + (sizeof size) + sizeof(long), List);
  return Ok;
}

on(Connect) {
  struct client* client = find_client_by_id(payload->id);
  if (client == NULL) return bad_qid;
  if (client->peer != NULL) return self_occupied;

  struct client* peer = find_client_by_id(payload->peer_id);
  if (peer == NULL) return bad_id;
  if (peer->peer != NULL) return occupied;

  if (client == peer) return self;
  client->peer = peer;
  peer->peer = client;

  sendi(Connect, client->qid, { peer->id, peer->pid });
  sendi(Connect, peer->qid, { client->id, client->pid });
  return Ok;
}

void on_error(id_t id, enum error_t error) {
  client* c = find_client_by_id(id);
  if (c) sendi(Error, c->qid, error);
}
#endif