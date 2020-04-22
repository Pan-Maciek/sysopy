#ifndef message_h
#define message_h

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include <stdbool.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE 70 

typedef int qid_t;

typedef enum msg_t {
  Stop = 10,
  Disconnect = 9,
  List = 8,
  Connect = 7,
  Init = 6,
  Error = 5,
  Message = 4
} msg_t;

#define parse(x) if (strcmp(c, #x) == 0) return x;
enum msg_t parse_msg_t(char* c) {
  if (strcmp(c, "exit") == 0) return Stop;
  parse(Stop)
  parse(Disconnect)
  if (strcmp(c, "ls") == 0) return List;
  parse(List)
  parse(Connect)
  parse(Message)
  if (strcmp(c, "Send") == 0) return Message;
  if (strcmp(c, "echo") == 0) return Message;
  return Error;
}
#undef parse

typedef enum error_t {
  Ok = 0,
  server_full,
  bad_id,
  bad_qid,
  occupied,
  self_occupied,
  self
} error_t;

struct message {
  unsigned int mtype;
  union payload {

    union request {
      struct { id_t id; } Init, Stop, Disconnect, List;
      struct { id_t id; id_t peer_id; } Connect;
      char Message[MAX_MESSAGE];
    } request;

    union response {
      struct { id_t id; } Init;
      struct { id_t peer_id; pid_t peer_pid; } Connect;
      struct { } Stop, Disconnect;
      struct { 
        int size; 
        struct client_info { id_t id; bool available; } clients[MAX_CLIENTS]; 
      } List;
      char Message[MAX_MESSAGE];
      enum error_t Error;
    } response;

  } payload;
};
typedef struct message message;

#if defined(CLIENT)
#define _SND_T request
#define _RCV_T response
#define handle(type) case type: on_##type(&msg.payload._RCV_T.type); break;
#elif defined(SERVER)
#define _SND_T response
#define _RCV_T request
enum error_t __error;
void on_error(id_t id, enum error_t error);
#define handle(type) case type: if ((__error = on_##type(&msg.payload._RCV_T.type)) != Ok) { on_error(msg.payload._RCV_T.type.id, __error); } break;
#endif

#define sendi(type, qid, init...) ({\
  struct message msg = { type };\
  typeof(msg.payload._SND_T.type) payload = init;\
  msg.payload._SND_T.type = payload;\
  mq_send(qid, (char*) &msg, sizeof payload + sizeof(enum msg_t), type);\
})

#define send0(type, qid) ({\
  struct message msg = { type };\
  mq_send(qid, (char*) &msg, sizeof(enum msg_t), type);\
})

#define QUEUE_ATTR ({\
  struct mq_attr info;\
  info.mq_msgsize = sizeof(message);\
  info.mq_maxmsg = 10;\
  &info;\
})

message msg;
#define on(type) enum error_t on_##type(typeof(msg.payload._RCV_T.type)* payload) 
#define recive(qid) if (mq_receive(qid, (char*) &msg, sizeof msg, NULL) == -1) { } else 
#define recive_async(qid) for (;mq_receive(qid, (char*) &msg, sizeof msg, NULL) != -1;)

#endif