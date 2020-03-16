#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

typedef unsigned int uint;

#define config(setting, value) {\
  config_##setting = value;\
  config_use_##setting = true;\
}

#define exit 1
#define cont 0

static uint config_max_depth = 1;
static time_t config_atime;
static time_t config_mtime;
static bool config_use_atime = false;
static bool config_use_mtime = false;
static bool config_use_max_depth = false;
static char config_mtime_mode;
static char config_atime_mode;

static time_t launch_time;

char* file_type_name(const struct stat* s) {
  uint mode = s->st_mode;
  if (S_ISDIR(mode)) return "dir";
  if (S_ISREG(mode)) return "file";
  if (S_ISLNK(mode)) return "link";
  if (S_ISCHR(mode)) return "char";
  if (S_ISBLK(mode)) return "block";
  if (S_ISFIFO(mode)) return "fifo";
  if (S_ISSOCK(mode)) return "socket";
  return "undefined";
}

void print_file(const char* path, const struct stat* s) {
  static const char* time_format = "%Y-%m-%d %H:%M:%S";
  static char m_time_str[20], a_time_str[20];
  struct tm m_time, a_time;

  localtime_r(&s->st_mtime, &m_time);
  localtime_r(&s->st_atime, &a_time);

  strftime(m_time_str, 20, time_format, &m_time);
  strftime(a_time_str, 20, time_format, &a_time);

  printf("%s\t%lu\t%s\t%ld\t%s\t%s\n", 
    path, s->st_nlink, 
    file_type_name(s), s->st_size,  
    a_time_str, m_time_str
  );
}

bool match_file(const char* path, const struct stat* s) {
  if (config_use_mtime) {
    if (config_mtime_mode == '+' && difftime(s->st_mtime, config_mtime) > 0) return false;
    if (config_mtime_mode == '-' && difftime(s->st_mtime, config_mtime) < 0) return false;
  }
  if (config_use_atime) {
    if (config_atime_mode == '+' && difftime(s->st_atime, config_atime) > 0) return false;
    if (config_atime_mode == '-' && difftime(s->st_atime, config_atime) < 0) return false;
  }
  return true;
}

char* configure(int argc, char** argv) {
  time(&launch_time);
  struct tm *timeinfo;

  char* search_path = argc >= 1 ? argv[0] : ".";
  for (int i = 1; i < argc; i++) {

    if (strcmp(argv[i], "-maxdepth") == 0) 
      config(max_depth, atoi(argv[++i]))

    else if (strcmp(argv[i], "-atime") == 0) {
      int arg = atoi(argv[++i]);
      timeinfo = localtime(&launch_time);
      timeinfo->tm_mday -= abs(arg);
      if (arg > 0) timeinfo->tm_mday -= 1;
      config_atime_mode = arg >= 0 ? '+' : '-';

      config(atime, mktime(timeinfo));
    }

    else if (strcmp(argv[i], "-mtime") == 0) {
      int arg = atoi(argv[++i]);
      timeinfo = localtime(&launch_time);
      timeinfo->tm_mday -= abs(arg);
      if (arg > 0) timeinfo->tm_mday -= 1;
      config_mtime_mode = arg >= 0 ? '+' : '-';

      config(mtime, mktime(timeinfo));
    }
  }

  return search_path;
}

int filter(const char* path, const struct stat* s, int type, struct FTW* f) {
  if (config_use_max_depth && type == FTW_D && f->level > config_max_depth) return exit;
  if (!match_file(path, s)) return exit;
  print_file(path, s);
  return cont;
}

#define find(path) nftw(path, filter, 0, 0)

int main(int argc, char** argv) {
  char* search_path = configure(argc - 1, argv + 1);
  find(search_path);
  return 0;
}