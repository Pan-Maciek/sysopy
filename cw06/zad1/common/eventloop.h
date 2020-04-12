#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <signal.h>

#define reg(type)                                                              \
  case type:                                                                   \
    if ((err = on_##type((typeof(msg.payload.type) *)&msg.payload))) {           \
    } break;
