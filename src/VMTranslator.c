// definitions {{{1
#include <ctype.h>
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
#define MAX_TOKEN_LENGTH 16
#define MAX_CONSTANT 32767
#define MAX_FILE_NAME 256
#define DT_REG 8
#define SLASH '/'

#define EXIT_ERROR(t)                                                          \
  do {                                                                         \
    fprintf(stderr, "Error on line %zu: %s\n", lineNumber, t);                 \
    return EXIT_FAILURE;                                                       \
  } while (0)

void sys_init(FILE *ofile, char const *fname);
void parse_file(FILE *file, FILE *ofile, char const *fname);
int write_command(char const *command, char const *arg1, char const *arg2,
                  char const *fname, size_t const lineNumber, FILE *output);
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
      char fname[MAX_FILE_NAME] = {0};
      char *dot = strrchr(path, '.');
      if (dot && !strcmp(dot, ".vm")) {
        *dot = '\0';
        strncpy(fname, slash + 1, sizeof(fname) - 1);
        strcpy(dot, ".asm");
        FILE *ofile = fopen(path, "w");
        if (ofile) {
          parse_file(file, ofile, fname);
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
        snprintf(file_path, PATH_MAX - 1, "%s%c%s.asm", path, SLASH, dname);
        FILE *ofile = fopen(file_path, "w");
        if (ofile) {
          sys_init(ofile, dname);
          while ((entry = readdir(dir))) {
            if (entry->d_type == DT_REG) {
              char *dot = strrchr(entry->d_name, '.');
              if (dot && !strcmp(dot, ".vm")) {
                snprintf(file_path, PATH_MAX - 1, "%s%c%s", path, SLASH,
                         entry->d_name);
                FILE *file = fopen(file_path, "r");
                if (file) {
                  *dot = '\0';
                  parse_file(file, ofile, entry->d_name);
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
// sys_init {{{1
void sys_init(FILE *ofile, char const *fname) {
  fprintf(ofile, "// %s\n\n", fname);
  fprintf(ofile, "// Bootstrap Sys.init\n");
  fprintf(ofile, "@256\n");
  fprintf(ofile, "D=A\n");
  fprintf(ofile, "@SP\n");
  fprintf(ofile, "M=D\n");
  fprintf(ofile, "@Sys.init\n");
  fprintf(ofile, "0;JMP\n");
}
// parse_file {{{1
void parse_file(FILE *file, FILE *ofile, char const *fname) {
  fprintf(ofile, "\n// %s\n", fname);
  char line[MAX_LINE_LENGTH] = {0};
  for (size_t lineNumber = 1; fgets(line, sizeof(line), file); lineNumber++) {
    char token[MAX_LINE_LENGTH] = {0};
    char command[MAX_TOKEN_LENGTH] = {0};
    char arg1[MAX_TOKEN_LENGTH] = {0};
    char arg2[MAX_TOKEN_LENGTH] = {0};
    size_t j = 0;

    for (size_t i = 0; i < strlen(line); i++) {
      char c = line[i];
      if (isspace(c) || c == '/') {
        if (*token) {
          token[j] = '\0';
          if (!*command) {
            strcpy(command, token);
          } else if (!*arg1) {
            strcpy(arg1, token);
          } else {
            strcpy(arg2, token);
            break;
          }
          *token = '\0';
          j = 0;
        }
        if (c == '/')
          break;
      } else {
        token[j++] = c;
      }
    }

    if (*command) {
      fprintf(ofile, "// %s", line);
      if (write_command(command, arg1, arg2, fname, lineNumber, ofile))
        break;
    }
  }
}
// write_command {{{1
int write_command(char const *command, char const *arg1, char const *arg2,
                  char const *fname, size_t const lineNumber, FILE *output) {
  int c = atoi(arg2);
  if (c < 0 || c > MAX_CONSTANT) {
    EXIT_ERROR("Address out of bounds");
  }
  // push {{{2
  if (!strcmp(command, "push")) {
    if (!strcmp(arg1, "constant")) {
      fprintf(output, "\t@%d\n", c);
      fprintf(output, "\tD=A\n");
    } else if (!strcmp(arg1, "static")) {
      fprintf(output, "\t@%s.%d\n", fname, c);
      fprintf(output, "\tD=M\n");
    } else if (!strcmp(arg1, "temp")) {
      fprintf(output, "\t@%d\n", c);
      fprintf(output, "\tD=A\n");
      fprintf(output, "\t@5\n");
      fprintf(output, "\tA=D+A\n");
      fprintf(output, "\tD=M\n");
    } else if (!strcmp(arg1, "pointer")) {
      (c) ? fprintf(output, "\t@THAT\n") : fprintf(output, "\t@THIS\n");
      fprintf(output, "D=M\n");
    } else {
      char symbol[5] = {0};
      if (!strcmp(arg1, "local")) {
        strcpy(symbol, "LCL");
      } else if (!strcmp(arg1, "argument")) {
        strcpy(symbol, "ARG");
      } else if (!strcmp(arg1, "this")) {
        strcpy(symbol, "THIS");
      } else if (!strcmp(arg1, "that")) {
        strcpy(symbol, "THAT");
      } else {
        EXIT_ERROR("Invalid segment reference");
      }
      fprintf(output, "\t@%d\n", c);
      fprintf(output, "\tD=A\n");
      fprintf(output, "\t@%s\n", symbol);
      fprintf(output, "\tA=D+M\n");
      fprintf(output, "\tD=M\n");
    }
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    // pop {{{2
  } else if (!strcmp(command, "pop")) {
    if (!strcmp(arg1, "static")) {
      fprintf(output, "\t@SP\n");
      fprintf(output, "\tAM=M-1\n");
      fprintf(output, "\tD=M\n");
      fprintf(output, "\t@%s.%s\n", fname, arg2);
      fprintf(output, "\tM=D\n");
    } else {
      if (!strcmp(arg1, "temp") || !strcmp(arg1, "pointer")) {
        fprintf(output, "\t@%d\n", c);
        fprintf(output, "\tD=A\n");
        fprintf(output, "\t@%d\n", (strcmp(arg1, "temp")) ? 3 : 5);
        fprintf(output, "\tD=D+A\n");
      } else {
        char symbol[5] = {0};
        if (!strcmp(arg1, "local")) {
          strcpy(symbol, "LCL");
        } else if (!strcmp(arg1, "argument")) {
          strcpy(symbol, "ARG");
        } else if (!strcmp(arg1, "this")) {
          strcpy(symbol, "THIS");
        } else if (!strcmp(arg1, "that")) {
          strcpy(symbol, "THAT");
        } else {
          EXIT_ERROR("Invalid segment reference");
        }
        fprintf(output, "\t@%d\n", c);
        fprintf(output, "\tD=A\n");
        fprintf(output, "\t@%s\n", symbol);
        fprintf(output, "\tD=D+M\n");
      }
      fprintf(output, "\t@R13\n");
      fprintf(output, "\tM=D\n");
      fprintf(output, "\t@SP\n");
      fprintf(output, "\tAM=M-1\n");
      fprintf(output, "\tD=M\n");
      fprintf(output, "\t@R13\n");
      fprintf(output, "\tA=M\n");
      fprintf(output, "\tM=D\n");
    }
    // add {{{2
  } else if (!strcmp(command, "add")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M-1\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M-1\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=D+M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // sub {{{2
  } else if (!strcmp(command, "sub")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M-1\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M-1\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=M-D\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // neg {{{2
  } else if (!strcmp(command, "neg")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=-M\n");
    // eq {{{2
  } else if (!strcmp(command, "eq")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@EQ_%zu\n", lineNumber);
    fprintf(output, "\tD;JEQ\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@CONT_%zu\n", lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(EQ_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // gt {{{2
  } else if (!strcmp(command, "gt")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@GT_%zu\n", lineNumber);
    fprintf(output, "\tD;JGT\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@CONT_%zu\n", lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(GT_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // lt {{{2
  } else if (!strcmp(command, "lt")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@LT_%zu\n", lineNumber);
    fprintf(output, "\tD;JLT\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@CONT_%zu\n", lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(LT_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // and {{{2
  } else if (!strcmp(command, "and")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D&M\n");
    // or {{{2
  } else if (!strcmp(command, "or")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D|M\n");
    // not {{{2
  } else if (!strcmp(command, "not")) {
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=!M\n");

    // }}}2
  } else {
    EXIT_ERROR("Invalid command");
  }
  return EXIT_SUCCESS;
}
