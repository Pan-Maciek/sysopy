#ifndef logic_c
#define logic_c

#define CLIENT
#include "../common/common.h"
#include "../common/message.h"

qid_t client_qid, server_qid;
qid_t peer_qid, peer_id;
id_t client_id = 0;

void disconnect() { peer_qid = peer_id = 0; }

#endif