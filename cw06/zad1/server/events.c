#include "../common/message.h"
#include "logic.c"

on(Init) {
  printf("[server] Init\n");
  client *client = new_client(payload->client_qid);
  return client ? sendi(Init, {client->id}, 1) : Server_full;
}

on(Stop) {
  printf("[server] Stop\n");
  return remove_client(payload->client_qid), Ok;
}

on(Disconnect) { 
  printf("[server] Disconnect\n");
  return disconnect(payload->client_qid), Ok;
}

on(ListReq) {
  printf("[server] ListsReq\n");
  message message = { List };
  int size = 0;

  for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    if (clients[i].qid != 0 && clients[i].qid != payload->client_qid) {
      message.payload.List.clients[size].id = clients[i].id;
      message.payload.List.clients[size].available = clients[i].peer == NULL;
      size++;
    }

  message.payload.List.size = size;
  msgsnd(payload->client_qid, &message, sizeof message.payload, 0);
  return Ok;
}