#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVER_KEY_PATHNAME "/home/maciek/"
#define PROJECT_ID 'M'

#define MAX_CLIENT_COUNT 10
#define QUEUE_PERMISSIONS 0640
#define MSG_SIZE 100

typedef int qid_t;
typedef enum msg_t {
  Stop = 1,
  Disconnect = 2,
  List = 3, 
  ListReq = 3,
  Error,
  Connect,
  Init,
  Id,
} msg_t;

typedef enum error_t { InternalError = -1, Ok = false, Server_full } error_t;

typedef struct message {
  long mtype;
  union payload {
    struct SimpleRequest { qid_t client_qid; }
    Init, Stop, Disconnect, ListReq;
    struct Error { enum error_t error; } Error;
    struct List { int size; struct { int id; bool available; } clients[MAX_CLIENT_COUNT]; } List;
  } payload;
} message;

message msg;
#define receive(qid)                                                           \
  for (enum error_t err;;)                                                     \
    if (msgrcv(qid, &msg, sizeof msg, -100, 0) == -1) {               \
    } else switch (msg.mtype)
#define sendi(type, init, qid)                                                 \
  ({                                                                           \
    struct message message = {type, {init}};                                   \
    msgsnd(qid, &message, sizeof message.payload.type, 0);                     \
  })
#define on(type) enum error_t on_##type(typeof(msg.payload.type) *payload)

#endif
