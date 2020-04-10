#include "common.h"
#include "message.h"

static int new_client_id = 1;
#define new_id new_client_id++
#define MAX_CLIENT_COUNT 10

struct client {
  int client_qid;
  int client_id;
} clients[MAX_CLIENT_COUNT];

qid_t server_qid;
void finish() { 
  msgctl(server_qid, IPC_RMID, NULL); 
}

void initialize() {
  key_t key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(key != -1);

  server_qid = msgget(key, IPC_CREAT | QUEUE_PERMISSIONS);
  assert(server_qid != -1);
}

#define find(xs, expr, size) for (i = 0; i < size; i++) { if (expr) break; } if (i < size)

int main() {
  INIT;

  int i;
  repeat receive(msg, server_qid) {
    switch (msg.mtype) {
    case Init: // add new client
      find(clients, clients[i].client_qid == 0, MAX_CLIENT_COUNT) {
        clients[i].client_qid = msg.payload.Init.client_qid;
        clients[i].client_id = new_id;
        send(Id, { clients[i].client_id }, clients[i].client_qid);
      }
      else send(Error, { server_full }, msg.payload.Init.client_qid);
      break;
    case Stop: // remove client
      find(clients, clients[i].client_qid == msg.payload.Stop.client_qid, MAX_CLIENT_COUNT) {
        clients[i].client_qid = 0;
      }
      break;
    }
  }
}
