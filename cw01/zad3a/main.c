#include "lib.h"
#include <stdio.h>
#include <sys/times.h>
#ifdef dynamic 
#include <dlfcn.h>
#endif

static struct tms tms_start, tms_end;
static clock_t clock_start, clock_end;

#define time_it(name, code_block)\
  clock_start = times(&tms_start);\
  code_block\
  clock_end = times(&tms_end);\
  printf("\n%s\n", name);\
  printf("real time: %ld\n", clock_end - clock_start);\
  printf(" sys time: %ld\n", tms_end.tms_stime - tms_start.tms_stime);\
  printf("user time: %ld\n", tms_end.tms_utime - tms_start.tms_utime)

int main(int argc, char **argv) {

  #ifdef dynamic
  void* handle = dlopen("./libdiff.so", RTLD_NOW);
  if( !handle ) {
    fprintf(stderr, "dlopen() %s\n", dlerror());
    exit(1);
  }
  table* (*create_table)(int) = dlsym(handle, "create_table");
  void (*compare_file_sequence)(table*, char**) = dlsym(handle, "compare_file_sequence");
  void (*remove_block)(table*, int) = dlsym(handle, "remove_block");
  void (*remove_operation)(table*, int, int) = dlsym(handle, "remove_operation");
  #endif

  table* main_table = NULL;

  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];

    if(strcmp(arg, "create_table") == 0) {
      if (main_table) free(main_table);
      int size = atoi(argv[++i]);
      main_table = create_table(size);
    }

    else if (strcmp(arg, "compare_pairs") == 0) {
      time_it("compare_pairs", {
        compare_file_sequence(main_table, argv + i + 1);
      });
      i = i + main_table->size;
    }

    else if (strcmp(arg, "remove_block") == 0) {
      int block_index = atoi(argv[++i]);
      time_it("remove_block", {
        remove_block(main_table, block_index);
      });
    }

    else if (strcmp(arg, "remove_operation") == 0) {
      int block_index = atoi(argv[++i]);
      int operation_index = atoi(argv[++i]);
      time_it("remove_operation", {
        remove_operation(main_table, block_index, operation_index);
      });
    }

  }
  return 0;
}