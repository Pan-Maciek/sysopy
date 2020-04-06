#ifndef COMMON_H
#define COMMON_H

#include <errno.h>
#include <string.h>
#define panic(str, args...) {\
  fprintf(stderr, "%s:%i ", __func__, __LINE__);\
  fprintf(stderr, str, args);\
  fprintf(stderr, ".\n");\
  exit(1);\
}

#endif