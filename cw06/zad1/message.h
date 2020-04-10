#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SERVER_KEY_PATHNAME "/home/maciek/"
#define PROJECT_ID 'M'

#define QUEUE_PERMISSIONS 0640
#define MSG_SIZE 100

typedef int qid_t;
typedef enum msg_t {
Stop = 0,
Dissconnect = 1,
  List = 2,
  Error,
  Connect,
  Init,
  Id,
} msg_t;

enum error { server_full };

typedef struct message {
  long mtype;
  union payload {
    struct Init { qid_t client_qid; } Init;
    struct Stop { qid_t client_qid; } Stop;
    struct Id { int id; } Id;
    struct Error { enum error error; } Error; 
  } payload;
} message;

#define repeat for (;;)
#define receive(msg, qid)                                                      \
  for (struct message msg;;)                                                   \
    if (msgrcv(qid, &msg, sizeof msg, -100, IPC_NOWAIT) == -1) { } else
#define send(type, init, qid)                                                  \
  {                                                                            \
    struct message message = {type, {init}};                                   \
    msgsnd(qid, &message, sizeof(struct type), 0);                             \
  }

#endif
