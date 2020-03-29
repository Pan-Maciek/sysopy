#ifndef IPC
#define IPC

#include <stdbool.h>
#include <unistd.h>

#define PARENT_OK SIGUSR1
#define CHILD_OK SIGUSR2

#ifdef IPC_MAIN

bool parent_ok, child_ok;
#define CHILD_STATUS (child_ok ? "true" : "false")
#define PARENT_STATUS (parent_ok ? "true" : "false")

static void on_parent_ok(int _) { parent_ok = true; }
static void on_child_ok(int _) { child_ok = true; }

#define IPC_INIT \
signal(PARENT_OK, on_parent_ok);\
signal(CHILD_OK, on_child_ok);

#endif

#endif