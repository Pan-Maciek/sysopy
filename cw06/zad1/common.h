#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <signal.h>
#include <assert.h>

void initialize();
void finish();
static void _finish(int _) { exit(0); }

#define INIT { signal(SIGINT, _finish); atexit(finish); initialize(); }