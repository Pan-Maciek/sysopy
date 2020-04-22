#ifndef message_h
#define message_h

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include <stdbool.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE 100 

typedef int qid_t;

typedef enum msg_t {
  Stop = 1L,
  Disconnect = 2L,
  List = 3L,
  Connect,
  Init,
  Error,
  Message
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
  long mtype;
  union payload {

    union request {
      struct { qid_t qid; } Init, Stop, Disconnect, List;
      struct { qid_t qid; id_t peer_id; } Connect;
      char Message[MAX_MESSAGE];
    } request;

    union response {
      struct { id_t id; } Init;
      struct { id_t peer_id; qid_t peer_qid; } Connect;
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

#define sendi(type, qid, init...) ({\
  message msg = { type };\
  typeof(msg.payload._SND_T.type) res = init;\
  msg.payload._SND_T.type = res;\
  msgsnd(qid, &msg, sizeof res, 0);\
})
#if defined(CLIENT)
#define _SND_T request
#define _RCV_T response
#define handle(type) case type: on_##type(&msg.payload._RCV_T.type); break;
#elif defined(SERVER)
#define _SND_T response
#define _RCV_T request
enum error_t __error;
#define handle(type) case type: if ((__error = on_##type(&msg.payload._RCV_T.type)) != Ok) { on_error(msg.payload._RCV_T.type.qid); } break;
void on_error(qid_t qid) {
  sendi(Error, qid, __error);
}
#endif


#define send0(type, qid) ({ message msg = { type };\
  msgsnd(qid, &msg, 0, 0); })

struct message msg;
#define on(type) enum error_t on_##type(typeof(msg.payload._RCV_T.type)* payload) 
#define recive(qid) if (msgrcv(qid, &msg, sizeof msg.payload, -100, 0) == -1) { } else 
#define recive_async(qid) for (;msgrcv(qid, &msg, sizeof msg.payload, -100, IPC_NOWAIT) != -1;)

#endif