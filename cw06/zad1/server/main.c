#include "events.c"
#include "../common/eventloop.h"

qid_t server_qid;

void finish() {
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) 
        if (clients[i].qid) sendi(Stop, { server_qid }, clients[i].qid);
    msgctl(server_qid, IPC_RMID, NULL);
}

int main() {
    key_t key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
    assert(key != -1);

    server_qid = msgget(key, IPC_CREAT | IPC_EXCL | QUEUE_PERMISSIONS);
    assert(server_qid != -1);

    atexit(finish);
    signal(SIGINT, exit);

    receive (server_qid) {
        reg(Init)
        reg(Stop)
        reg(ListReq)
        reg(Disconnect)
    }
}
