#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>

#define SERVER_KEY_PATHNAME "/home/maciek/"
#define PROJECT_ID 'M'

#define QUEUE_PERMISSIONS 0640

typedef enum msg_type {
  List = 'L',
  Connect = 'C',
  Dissconnect = 'D',
  Stop = 'S'
} msg_type;

typedef struct message {
  enum msg_type type;
  key_t qid;
} message;

#define as ,
#define repeat for(;;)
#define case(x, code) case x: {code;} break;
#define recive(qid, msg) if (msgrcv(qid, &msg, sizeof msg, 0, 0) == -1) exit(1); else 
#define then
#define async if (fork() == 0)

#endif