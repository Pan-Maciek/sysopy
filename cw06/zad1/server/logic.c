#include "../common/message.h"

struct client {
  qid_t qid;
  int id;
  struct client *peer;
} clients[MAX_CLIENT_COUNT];
typedef struct client client;

client *new_client(qid_t qid) {
  static int id = 1;
  for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    if (clients[i].qid == 0) {
      client *c = clients + i;
      c->qid = qid;
      c->id = id++;
      c->peer = NULL;
      return c;
    }
  return NULL;
}

static void disconnect_pear(client* c) {
  if (c->peer == NULL) return;
  sendi(Disconnect, {c->qid}, c->peer->qid);
  c->peer->peer = NULL;
  c->peer = NULL;
}

void remove_client(qid_t qid) {
  for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if (clients[i].qid == qid) {
      disconnect_pear(clients + i);
      clients[i].qid = 0;
    }
  }
}

void disconnect(qid_t qid) {
  for (int i = 0; i < MAX_CLIENT_COUNT; i++) 
    if (clients[i].qid == qid) 
      disconnect_pear(clients + i);
}