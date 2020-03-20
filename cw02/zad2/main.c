#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>
#ifdef use_nftw
#include <ftw.h>
#endif

typedef unsigned int uint;

#define config(setting, value) {\
  config_##setting = value;\
  config_use_##setting = true;\
}

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

#ifndef use_nftw
char* join_path(char* path1, char* path2) {
  char* path = malloc(sizeof(char) * (strlen(path1) + strlen(path2)) + 2);
  sprintf(path, "%s/%s", path1, path2);
  return path;
}

static void scan_dir(char* path, int depth) {
  if (!path || (config_use_max_depth && depth >= config_max_depth)) return;
  DIR* dir = opendir(path);
  if (!dir) return; // can not read

  struct dirent* d;
  struct stat s;
  while ((d = readdir(dir))) {
    if (strcmp(d->d_name, "..") == 0) continue;
    if (strcmp(d->d_name, ".") == 0) continue;

    char* dir = join_path(path, d->d_name);
    if (lstat(dir, &s) < 0) continue; // can not read
    if (S_ISDIR(s.st_mode)) scan_dir(dir, depth + 1);

    if (match_file(dir, &s)) print_file(dir, &s);
    free(dir);
  }
  closedir(dir);
}
#endif

#ifdef use_nftw
int filter(const char* path, const struct stat* s, int type, struct FTW* f) {
  if (config_use_max_depth && type == FTW_D && f->level > config_max_depth) return 1;
  if (!match_file(path, s)) return 1;
  print_file(path, s);
  return 0;
}
#endif

void find(char* path) {
  #ifndef use_nftw
  struct stat s;
  if (lstat(path, &s) >= 0 && match_file(path, &s)) 
    print_file(path, &s);
  scan_dir(path, 0);
  #endif
  #ifdef use_nftw
  nftw(path, filter, 0, 0);
  #endif
}

#define opt_atime_i    0
#define opt_mtime_i    1
#define opt_maxdepth_i 2

#define case(name, code)  case opt_##name##_i: code; break

const struct option options[] = {
  {"atime",    required_argument, 0, 'a'},
  {"mtime",    required_argument, 0, 'm'},
  {"maxdepth", required_argument, 0,  0 },
  {0, 0, 0, 0}
};

int main(int argc, char** argv) {
  char* search_path = argc > 1 ? argv[1] : ".";

  time(&launch_time);
  struct tm* timeinfo;
  for (int option; getopt_long_only(argc, argv, "", options, &option) >= 0;) {
    switch (option) {
      case(atime, {
        int arg = atoi(optarg);
        timeinfo = localtime(&launch_time);
        timeinfo->tm_mday -= abs(arg);
        if (arg > 0) timeinfo->tm_mday -= 1;
        config_atime_mode = arg >= 0 ? '+' : '-';

        config(atime, mktime(timeinfo));
      });
      case(mtime, {
        int arg = atoi(optarg);
        timeinfo = localtime(&launch_time);
        timeinfo->tm_mday -= abs(arg);
        if (arg > 0) timeinfo->tm_mday -= 1;
        config_mtime_mode = arg >= 0 ? '+' : '-';

        config(mtime, mktime(timeinfo));
      });
      case(maxdepth, config(max_depth, atoi(optarg)) );
    }
  }
  find(search_path);
  return 0;
}