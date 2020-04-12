#include "events.c"
#include "../common/eventloop.h"

void finish() {
    msgctl(client_qid, IPC_RMID, NULL);
    sendi(Stop, { client_qid }, server_qid);
}

int main() {
    key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
    assert(server_key != -1);

    server_qid = msgget(server_key, QUEUE_PERMISSIONS);
    assert(server_qid != -1);

    client_qid = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
    assert(client_qid != -1);

    atexit(finish);
    signal(SIGINT, exit);

    sendi(Init, { client_qid }, server_qid);
    sendi(ListReq, { client_qid }, server_qid);
    receive(client_qid) {
        reg(Init)
        reg(List)
        reg(Stop)
    }
}