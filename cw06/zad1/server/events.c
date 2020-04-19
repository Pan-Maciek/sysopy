#ifndef events_c
#define events_c

#include "logic.c"

on(Init) {
  client* client = new_client(payload->qid);
  return client ? sendi(Init, client->qid, { client->id }) : server_full;
}

on(Stop) {
  return remove_client(payload->qid), Ok;
}

on(Disconnect)  {
  return disconnect(payload->qid), Ok;
}

on(List) {
  message msg = { List };
  int size = 0;

  struct client_info* info;
  for (int i = 0; i < MAX_CLIENTS; i++)
    if (clients[i].qid) {
      info = msg.payload.response.List.clients + size;
      info->id = clients[i].id;
      info->available = clients[i].peer == NULL;
      size++;
    }
  
  msg.payload.response.List.size = size;
  assert(msgsnd(payload->qid, &msg, (sizeof *info) * size + (sizeof size), 0) != -1);
  return Ok;
}

on(Connect) {
  struct client* client = find_client_by_qid(payload->qid);
  if (client == NULL) return bad_qid;

  struct client* peer = find_client_by_id(payload->peer_id);
  if (peer == NULL) return bad_id;

  client->peer = peer;
  peer->peer = client;

  sendi(Connect, client->qid, { peer->id, peer->qid });
  sendi(Connect, peer->qid, { client->id, client->qid });
  return Ok;
}

#endif