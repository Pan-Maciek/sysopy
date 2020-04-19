#ifndef logic_c
#define logic_c

#define CLIENT
#include "../common/common.h"
#include "../common/message.h"

mqd_t client_qid, server_qid;
mqd_t peer_qid, peer_id;
pid_t peer_pid;
id_t client_id = 0;

void disconnect() { 
  if (peer_pid == 0) return;
  mq_close(peer_qid);
  peer_pid = peer_qid = peer_id = 0; 
}

#endif