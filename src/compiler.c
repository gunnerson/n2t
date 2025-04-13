//
// definitions {{{1
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 128
#define MAX_TOKEN_LENGTH 32
#define MAX_CONSTANT 32767
#define MAX_FILE_NAME 256
#define DT_REG 8
#define DT_UNKNOWN 0
#define SLASH '/'

#define STACK_ADDRESS 256U

extern char *realpath(const char *restrict path, char *restrict resolved_path);

// main {{{1
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *path = realpath(argv[1], NULL);
  if (path == NULL) {
    perror("Couldn't resolve path");
    return EXIT_FAILURE;
  }
  char *slash = strrchr(path, SLASH);
  if (slash == NULL) {
    perror("Error allocating memory");
    free(path);
    return EXIT_FAILURE;
  }

  struct stat *path_stat = malloc(sizeof(struct stat));
  if (path_stat == NULL) {
    perror("Error allocating memory");
    free(path);
    return EXIT_FAILURE;
  }
  if (stat(path, path_stat) == -1) {
    perror("Errot getting file attributes");
    free(path);
    return EXIT_FAILURE;
  }

  if (S_ISREG(path_stat->st_mode)) {
    FILE *file = fopen(path, "r");
    if (file) {
      char *dot = strrchr(path, '.');
      if (dot && !strcmp(dot, ".jack")) {
        strcpy(dot, ".vm");
        FILE *ofile = fopen(path, "w");
        if (ofile) {
          fclose(ofile);
        } else
          fprintf(stderr, "Error creating output file: %s\n", path);
      } else
        fprintf(stderr, "Invalid file path\n");

      fclose(file);
    } else
      perror("Error opening file");

  } else if (S_ISDIR(path_stat->st_mode)) {
    DIR *dir = opendir(path);
    if (dir) {
      struct dirent *entry;
      char dname[MAX_FILE_NAME] = {0};
      strncpy(dname, slash + 1, sizeof(dname) - 1);
      char *file_path = calloc(PATH_MAX, sizeof(char));
      if (file_path) {
        snprintf(file_path, PATH_MAX - 1, "%s%c%s.vm", path, SLASH, dname);
        FILE *ofile = fopen(file_path, "w");
        if (ofile) {
          while ((entry = readdir(dir))) {
            if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
              char *dot = strrchr(entry->d_name, '.');
              if (dot && !strcmp(dot, ".jack")) {
                snprintf(file_path, PATH_MAX - 1, "%s%c%s", path, SLASH,
                         entry->d_name);
                FILE *file = fopen(file_path, "r");
                if (file) {
                  fclose(file);
                } else
                  perror("Error opening file");
              }
            }
          }
          fclose(ofile);
        } else
          perror("Error opening out file");
        free(file_path);
      } else
        perror("Error allocating memory");
      closedir(dir);
    } else
      perror("Error opening directory");

  } else {
    perror("Error opening files");
  }
  free(path_stat);
  free(path);
  return (errno) ? EXIT_FAILURE : EXIT_SUCCESS;
}
