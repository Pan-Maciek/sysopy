#define SERVER
#include "../common/message.h"
#include "../common/common.h"

struct client {
  mqd_t qid;
  pid_t pid;
  id_t id;
  struct client *peer;
} clients[MAX_CLIENTS];
typedef struct client client;

client *new_client(pid_t pid) {
  static int id = 1;
  for (int i = 0; i < MAX_CLIENTS; i++)
    if (clients[i].qid == 0) {
      client *c = clients + i;
      c->qid = mq_open(path(pid), O_RDWR, QUEUE_PERMISSIONS, NULL);
      c->id = id++;
      c->pid = pid;
      c->peer = NULL;
      return c;
    }
  return NULL;
}

static void disconnect_pear(client* c) {
  if (c->peer == NULL) return;
  send0(Disconnect, c->peer->qid);
  send0(Disconnect, c->qid);
  c->peer->peer = NULL;
  c->peer = NULL;
}

void remove_client(id_t id) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].id == id) {
      disconnect_pear(clients + i);
      clients[i].id = 0;
      send0(Stop, clients[i].qid);
      mq_close(clients[i].qid);
      clients[i].pid = 0;
      clients[i].qid = 0;
      clients[i].peer = NULL;
    }
  }
}

void disconnect(id_t id) {
  for (int i = 0; i < MAX_CLIENTS; i++) 
    if (clients[i].id == id)
      disconnect_pear(clients + i);
}

client* find_client_by_id(id_t id) {
  for (int i = 0; i < MAX_CLIENTS; i++)
    if (clients[i].id == id)
      return clients + i;
  return NULL;
}

client* find_client_by_qid(qid_t qid) {
  for (int i = 0; i < MAX_CLIENTS; i++)
    if (clients[i].qid == qid)
      return clients + i;
  return NULL;
}