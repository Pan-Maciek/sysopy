#ifndef MACROS_H
#define MACROS_H

#include <pthread.h>
#include <time.h>

#define copy(arg) ({ typeof(arg)* copy = malloc(sizeof *copy); *copy = arg; copy; })
#define p_creat(thread_ptr, fn, arg) pthread_create(thread_ptr, NULL, (void* (*)(void*)) fn, copy(arg))
#define p_join(thread, data_ptr) pthread_join(thread, (void*) data_ptr);
#define ceil_div(a, b) (a / b) + (a % b ? 1 : 0)
#define min(a, b) ({ __auto_type A = a; __auto_type B = b; A < B ? A : B; })
#define repeat(n) for(int i = 0; i < n; ++i)

#define measure_time \
  int time;\
  for (\
    struct timespec _time_start, _time_end, *_run = (clock_gettime(CLOCK_MONOTONIC, &_time_start), NULL);\
    _run == NULL;\
    _run = (clock_gettime(CLOCK_MONOTONIC, &_time_end), time = (_time_end.tv_sec - _time_start.tv_sec) * 1000000 + (_time_end.tv_nsec - _time_start.tv_nsec) / 1000, (void*) 1)\
  )

#endif