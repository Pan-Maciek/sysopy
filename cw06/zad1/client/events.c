#include "../common/message.h"
#include "logic.c"

on(Init) {
    printf("[client] Init\n");
    client_id = payload->client_qid;
    return Ok;
}

on(Stop) {
    printf("[client] Stop\n");
    server_qid = -1;
    return exit(0), Ok;
}

on(List) {
    printf("[client] List\n");
    for (int i = 0, size = payload->size; i < size; i++) {
        printf("id(%d) available(%s)\n", payload->clients[i].id, payload->clients[i].available ? "T" : "F");
    }
    return Ok;
}