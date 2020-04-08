#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "message.h"

int main() {
  key_t key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
  assert(key != -1);

  int qid = msgget(key, IPC_CREAT | QUEUE_PERMISSIONS);
  assert(qid != -1);

  message msg;
  repeat recive (qid, msg) {
    msgsnd(msg.qid, &msg, sizeof msg, 0);
    switch (msg.type) {
      case(List, printf("List\n"))
      case(Connect, printf("Connect\n"))
      case(Dissconnect, printf("Dissconnect\n"))
      case(Stop, printf("Stop\n"))
    }
  }
  
}