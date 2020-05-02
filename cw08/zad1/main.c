#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "image.c"
#include "macros.h"


typedef struct thread_start_info {
  image* img;
  int start, step, stop;
} thread_start_info;

typedef struct thread_ret_value {
  int time;
  unsigned int* fragment;
} thread_ret_value;

unsigned int hist[SHADES];

enum thread_fn { sign, block, interleaved };
enum thread_fn parse(char* arg) {
  #define parse(x) if (strcmp(arg, #x) == 0) return x;
  parse(sign)
  parse(block)
  parse(interleaved)
  #undef parse
  return -1;
}

thread_ret_value* sign_fn(thread_start_info* info) {
  measure_time {
    shade** data = info->img->data, value;
    int width = info->img->width, height = info->img->height;
    int min = info->start, max = info->stop;

    for (int x = 0; x < width; ++x) {
      shade* coll = data[x];
      for (int y = 0; y < height; ++y) {
        value = coll[y];
        if (min <= value && value < max)
          ++hist[value];
      }
    }
  }
  
  thread_ret_value* ret = malloc(sizeof *ret);
  ret->time = time;
  ret->fragment = NULL;
  return ret;
}

thread_ret_value* block_fn(thread_start_info* info) {
  unsigned int*  count;
  measure_time {
    shade** data = info->img->data;
    count = calloc(SHADES, sizeof *count);
    int height = info->img->height;
    int stop = info->stop, step = info->step;

    for (int x = info->start; x < stop; x += step) {
      shade* coll = data[x];
      for (int y = 0; y < height; y++)
        ++count[coll[y]];
    }
  }

  thread_ret_value* ret = malloc(sizeof *ret);
  ret->fragment = count;
  ret->time = time;
  return ret;
}


int main(int argc, char** argv) {
  int thread_count = atoi(argv[1]);
  assert(thread_count > 0);

  image* img = load_image(argv[3]);
  assert(img != NULL);

  FILE* file_out = fopen(argv[4], "w+");
  assert(file_out != NULL);

  enum thread_fn fn = parse(argv[2]);

  int problem_size = fn == sign ? SHADES : img->width;
  int chunk = ceil_div(problem_size, thread_count);

  pthread_t *threads = malloc(thread_count * sizeof *threads);

  measure_time {
    if (fn == sign) {
      repeat (thread_count) {
        thread_start_info info = { img, chunk * i, 1, chunk * (i + 1) };
        p_creat(&threads[i], sign_fn, info);
      }
      repeat (thread_count) {
        thread_ret_value* ret;
        p_join(threads[i], &ret);
        printf("thread(%d) took(%d) microseconds\n", i, ret->time);
        free(ret);
      }
    } else {
      if (fn == block) {
        repeat (thread_count) {
          thread_start_info info = { img, chunk * i, 1, min(chunk * (i + 1), problem_size) };
          p_creat(&threads[i], block_fn, info);
        }
      } else if (fn == interleaved) {
        repeat (thread_count) {
          thread_start_info info = { img, i, thread_count, img->width };
          p_creat(&threads[i], block_fn, info);
        }
      }

      repeat (thread_count) {
        thread_ret_value* ret;
        p_join(threads[i], &ret);
        repeat (SHADES) hist[i] += ret->fragment[i];
        printf("thread(%d) took(%d) microseconds\n", i, ret->time);
        free(ret->fragment);
        free(ret);
      }
    }
  }

  repeat (SHADES) fprintf(file_out, "%d %u\n", i, hist[i]);
  printf("total time(%d) microseconds", time);
  fclose(file_out);
  return 0;
}