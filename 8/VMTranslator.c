//
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
#define MAX_SYMBOL_LENGTH 32
#define MAX_CONSTANT 32767
#define MAX_FILE_NAME 256
#define DT_REG 8
#define DT_UNKNOWN 0
#define SLASH '/'

#define EXIT_ERROR(t)                                                          \
  do {                                                                         \
    fprintf(stderr, "Error on line %zu: %s\n", lineNumber, t);                 \
    return EXIT_FAILURE;                                                       \
  } while (0)

void sys_init(FILE *ofile, char const *fname);
void parse_file(FILE *file, FILE *ofile, char const *fname,
                unsigned *commandNumber);
int write_command(char const *command, char const *arg1, char const *arg2,
                  char *foo_name, char const *fname, size_t const lineNumber,
                  unsigned *commandNumber, FILE *output);
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

  unsigned cn = 0;
  unsigned *commandNumber = &cn;

  if (S_ISREG(path_stat->st_mode)) {
    FILE *file = fopen(path, "r");
    if (file) {
      char fname[MAX_FILE_NAME] = {0};
      char *dot = strrchr(path, '.');
      if (dot && !strcmp(dot, ".vm")) {
        *dot = '\0';
        strncpy(fname, slash + 1, sizeof(fname) - 1);
        fname[MAX_SYMBOL_LENGTH - 1] = '\0';
        strcpy(dot, ".asm");
        FILE *ofile = fopen(path, "w");
        if (ofile) {
          parse_file(file, ofile, fname, commandNumber);
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
          *commandNumber += 4;
          char foo_name_init[9];
          write_command("call", "Sys.init", "0", foo_name_init, "", 0,
                        commandNumber, ofile);
          fprintf(ofile, "\n");
          while ((entry = readdir(dir))) {
            if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
              char *dot = strrchr(entry->d_name, '.');
              if (dot && !strcmp(dot, ".vm")) {
                snprintf(file_path, PATH_MAX - 1, "%s%c%s", path, SLASH,
                         entry->d_name);
                FILE *file = fopen(file_path, "r");
                if (file) {
                  *dot = '\0';
                  entry->d_name[MAX_SYMBOL_LENGTH - 1] = '\0';
                  parse_file(file, ofile, entry->d_name, commandNumber);
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
void sys_init(FILE *output, char const *fname) {
  fprintf(output, "// %s\n\n", fname);
  fprintf(output, "// [0] Bootstrap Sys.init\n");
  fprintf(output, "\t@256\n");
  fprintf(output, "\tD=A\n");
  fprintf(output, "\t@SP\n");
  fprintf(output, "\tM=D\n");
}
// parse_file {{{1
void parse_file(FILE *file, FILE *ofile, char const *fname,
                unsigned *commandNumber) {
  fprintf(ofile, "// %s\n", fname);
  char line[MAX_LINE_LENGTH];
  char foo_name[MAX_SYMBOL_LENGTH];
  for (size_t lineNumber = 1; fgets(line, sizeof(line), file); lineNumber++) {
    char command[16] = {0};
    char arg1[MAX_SYMBOL_LENGTH * 2] = "";
    char arg2[8] = {0};
    char *c = line;
    char token[sizeof(arg1)] = "";

    for (size_t i = 0; i < MAX_SYMBOL_LENGTH * 2; c++) {
      if (isspace(*c) || *c == '/' || *c == '\0' ||
          i == MAX_SYMBOL_LENGTH * 2 - 1) {
        if (*token) {
          token[i] = '\0';
          if (!*command) {
            strncpy(command, token, sizeof(command) - 1);
          } else if (!*arg1) {
            strcpy(arg1, token);
          } else {
            strncpy(arg2, token, sizeof(arg2) - 1);
            break;
          }
          *token = '\0';
          i = 0;
          if (!*c)
            strcpy(c, "\n\0");
        }
        if (*c == '/' || *c == '\0') {
          break;
        }
      } else if (*c) {
        token[i++] = *c;
      }
    }

    if (*command) {
      fprintf(ofile, "// [%d] %s %s %s\n", *commandNumber, command, arg1, arg2);
      if (write_command(command, arg1, arg2, foo_name, fname, lineNumber,
                        commandNumber, ofile))
        break;
    }
  }
  fprintf(ofile, "\n");
}
// write_command {{{1
int write_command(char const *command, char const *arg1, char const *arg2,
                  char *foo_name, char const *fname, size_t const lineNumber,
                  unsigned *commandNumber, FILE *output) {
  int c = atoi(arg2);
  if (c < 0 || c > MAX_CONSTANT) {
    EXIT_ERROR("Address out of bounds");
  }
  // push {{{2
  if (!strcmp(command, "push")) {
    if (!strcmp(arg1, "constant")) {
      *commandNumber += 6;
      fprintf(output, "\t@%d\n", c);
      fprintf(output, "\tD=A\n");
    } else if (!strcmp(arg1, "static")) {
      *commandNumber += 6;
      fprintf(output, "\t@%s.%d\n", fname, c);
      fprintf(output, "\tD=M\n");
    } else if (!strcmp(arg1, "temp")) {
      *commandNumber += 9;
      fprintf(output, "\t@%d\n", c);
      fprintf(output, "\tD=A\n");
      fprintf(output, "\t@5\n");
      fprintf(output, "\tA=D+A\n");
      fprintf(output, "\tD=M\n");
    } else if (!strcmp(arg1, "pointer")) {
      *commandNumber += 6;
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
      *commandNumber += 9;
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
      *commandNumber += 5;
      fprintf(output, "\t@SP\n");
      fprintf(output, "\tAM=M-1\n");
      fprintf(output, "\tD=M\n");
      fprintf(output, "\t@%s.%s\n", fname, arg2);
      fprintf(output, "\tM=D\n");
    } else {
      if (!strcmp(arg1, "temp") || !strcmp(arg1, "pointer")) {
        *commandNumber += 12;
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
        *commandNumber += 12;
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
    *commandNumber += 10;
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
    *commandNumber += 10;
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
    *commandNumber += 3;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=-M\n");
    // eq {{{2
  } else if (!strcmp(command, "eq")) {
    *commandNumber += 18;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@%s$__eq_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\tD;JEQ\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@%s$__cont_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(%s$__eq_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(%s$__cont_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // gt {{{2
  } else if (!strcmp(command, "gt")) {
    *commandNumber += 18;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@%s$__gt_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\tD;JGT\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@%s$__cont_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(%s$__gt_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(%s$__cont_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // lt {{{2
  } else if (!strcmp(command, "lt")) {
    *commandNumber += 18;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M-D\n");
    fprintf(output, "\t@%s$__lt_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\tD;JLT\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\t@%s$__cont_%zu__\n", foo_name, lineNumber);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(%s$__lt_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=-1\n");
    fprintf(output, "(%s$__cont_%zu__)\n", foo_name, lineNumber);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    // and {{{2
  } else if (!strcmp(command, "and")) {
    *commandNumber += 6;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D&M\n");
    // or {{{2
  } else if (!strcmp(command, "or")) {
    *commandNumber += 6;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D|M\n");
    // not {{{2
  } else if (!strcmp(command, "not")) {
    *commandNumber += 3;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=!M\n");
    // label {{{2
  } else if (!strcmp(command, "label")) {
    fprintf(output, "(%s$%s)\n", foo_name, arg1);
    // goto {{{2
  } else if (!strcmp(command, "goto")) {
    *commandNumber += 2;
    fprintf(output, "\t@%s$%s\n", foo_name, arg1);
    fprintf(output, "\t0;JMP\n");
    // if-goto {{{2
  } else if (!strcmp(command, "if-goto")) {
    *commandNumber += 5;
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@%s$%s\n", foo_name, arg1);
    fprintf(output, "\tD;JNE\n");
    // call {{{2
  } else if (!strcmp(command, "call")) {
    strcpy(foo_name, arg1);
    *commandNumber += 41;
    fprintf(output, "\t@%s$__return_%u__\n", foo_name, *commandNumber);
    fprintf(output, "\tD=A\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@LCL\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@ARG\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@THIS\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@THAT\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\tD=A+1\n");
    fprintf(output, "\t@%d\n", c + 5);
    fprintf(output, "\tD=D-A\n");
    fprintf(output, "\t@ARG\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@LCL\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@%s\n", foo_name);
    fprintf(output, "\t0;JMP\n");
    fprintf(output, "(%s$__return_%u__)\n", foo_name, *commandNumber);
    // function {{{2
  } else if (!strcmp(command, "function")) {
    strcpy(foo_name, arg1);
    *commandNumber += 11;
    fprintf(output, "(%s)\n", foo_name);
    fprintf(output, "\t@%s\n", arg2);
    fprintf(output, "\tD=A\n");
    fprintf(output, "\t@%s$__endinit__\n", foo_name);
    fprintf(output, "\tD;JEQ\n");
    fprintf(output, "(%s$__init__)\n", foo_name);
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=M+1\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tM=0\n");
    fprintf(output, "\tD=D-1\n");
    fprintf(output, "\t@%s$__init__\n", foo_name);
    fprintf(output, "\tD;JGT\n");
    fprintf(output, "(%s$__endinit__)\n", foo_name);
    // return {{{2
  } else if (!strcmp(command, "return")) {
    *commandNumber += 41;
    fprintf(output, "\t@LCL\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@R14\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@5\n");
    fprintf(output, "\tA=D-A\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@R15\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@ARG\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\tD=A+1\n");
    fprintf(output, "\t@SP\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@R14\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@THAT\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@R14\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@THIS\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@R14\n");
    fprintf(output, "\tAM=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@ARG\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@R14\n");
    fprintf(output, "\tA=M-1\n");
    fprintf(output, "\tD=M\n");
    fprintf(output, "\t@LCL\n");
    fprintf(output, "\tM=D\n");
    fprintf(output, "\t@R15\n");
    fprintf(output, "\tA=M\n");
    fprintf(output, "\t0;JMP\n");
    // }}}2
  } else {
    EXIT_ERROR("Invalid command");
  }
  return EXIT_SUCCESS;
}
