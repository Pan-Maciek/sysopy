#include "message.h"
#include "common.h"

qid_t client_qid, server_qid;
int client_id;

void finish() {
  send(Stop, { client_qid }, server_qid);
  msgctl(client_qid, IPC_RMID, NULL);
}

void initialize() {
  key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(server_key != -1);

  server_qid = msgget(server_key, QUEUE_PERMISSIONS);
  assert(server_qid != -1);

  client_qid = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
  assert(client_qid != -1);
}

int main() {
  INIT;
  
  send(Init, { client_qid }, server_qid);
  receive(msg, client_qid) {
    client_id = msg.payload.Id.id;
    printf("client_id:\t%d\n", msg.payload.Id.id);
    break;
  }
}
