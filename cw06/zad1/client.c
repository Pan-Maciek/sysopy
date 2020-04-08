#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "message.h"

int main() {
  key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(server_key != -1);

  int server_qid = msgget(server_key, QUEUE_PERMISSIONS);
  assert(server_qid != -1);

  int client_qid = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
  assert(client_qid != -1);

  message msg = { 'L', client_qid };
  async repeat {
    scanf("%c", (char*) &msg.type);
    if (msg.type == '\n') continue;
    msgsnd(server_qid, &msg, sizeof msg, 0);
  }

  repeat recive(client_qid, msg) {
    printf("%c\n", msg.type);
    if (msg.type == Stop) exit(0);
  }
}