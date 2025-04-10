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

typedef struct Parser {
  bool debug;
  FILE *in;
  FILE *out;
  char const *fname;
  char *command;
  char *arg1;
  char *arg2;

  void (*prs_init)(bool debug, FILE *in, FILE *out, char const *fname,
                   char *command, char *arg1, char *arg2);
} prs;

void parse_file(FILE *file, FILE *ofile, char const *fname, bool debug);
int write_command(char const *command, char const *arg1, char const *arg2,
                  char const *fname, size_t const lineNumber, FILE *output);
char *realpath(const char *restrict path, char *restrict resolved_path);
// main {{{1
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  bool debug = false;
  if (argc >= 3) {
    for (size_t i = 2; i < argc; i++) {
      if (!strcmp(argv[i], "-d"))
        debug = true;
    }
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
          parse_file(file, ofile, fname, debug);
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
      char fname[MAX_FILE_NAME] = {0};
      strncpy(fname, slash + 1, sizeof(fname) - 1);
      while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
          char *dot = strrchr(entry->d_name, '.');
          if (dot && !strcmp(dot, ".vm")) {
            char *file_path = calloc(PATH_MAX, sizeof(char));
            if (file_path) {
              snprintf(file_path, PATH_MAX - 1, "%s%c%s", path, SLASH,
                       entry->d_name);
              FILE *file = fopen(file_path, "r");
              if (file) {
                snprintf(file_path, PATH_MAX - 1, "%s%c%s.asm", path, SLASH,
                         fname);
                FILE *ofile = fopen(file_path, "w");
                if (ofile) {
                  parse_file(file, ofile, fname, debug);
                  fclose(ofile);
                } else
                  perror("Error opening ofile");
                fclose(file);
              } else {
                perror("Error opening ifile");
              }
              free(file_path);
            } else {
              perror("Error allocating memory");
            }
          }
        }
      }
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
// cw {{{1
void cw(FILE *ofile, char const *command, char const *comment, bool debug) {}
// parse_file {{{1
void parse_file(FILE *file, FILE *ofile, char const *fname, bool debug) {
  fprintf(ofile, "// %s\n\n", fname);
  fprintf(ofile, "// Bootstrap Sys.init\n");
  fprintf(ofile, "@256\n");
  fprintf(ofile, "D=A\n");
  fprintf(ofile, "@SP\n");
  fprintf(ofile, "M=D\t\t// *SP = 256\n");
  fprintf(ofile, "@Sys.init\n");
  fprintf(ofile, "0;JMP\n");

  char line[MAX_LINE_LENGTH] = {0};
  for (size_t lineNumber = 1; fgets(line, MAX_LINE_LENGTH, file);
       lineNumber++) {
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
      fprintf(ofile, "// %s", command);
      if (*arg1)
        fprintf(ofile, " %s", arg1);
      if (*arg2)
        fprintf(ofile, " %s", arg2);
      fprintf(ofile, "\n");
      if (write_command(command, arg1, arg2, fname, lineNumber, ofile)) {
        break;
      }
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
      fprintf(output, "@%d\n", c);
      fprintf(output, "D=A\t\t// D = %d\n", c);
    } else if (!strcmp(arg1, "static")) {
      fprintf(output, "@%s.%d\n", fname, c);
      fprintf(output, "D=M\t\t// D = *%s.%d\n", fname, c);
    } else if (!strcmp(arg1, "temp")) {
      fprintf(output, "@%d\n", c);
      fprintf(output, "D=A\t\t// D = %d\n", c);
      fprintf(output, "@5\n");
      fprintf(output, "A=D+A\t\t// A += 5\n");
      fprintf(output, "D=M\t\t// D = *(D + 5)\n");
    } else if (!strcmp(arg1, "pointer")) {
      (c) ? fprintf(output, "@THAT\n") : fprintf(output, "@THIS\n");
      fprintf(output, "D=M\t\t// D = *(THIS/THAT)\n");
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
      fprintf(output, "@%d\n", c);
      fprintf(output, "D=A\t\t// D = %d\n", c);
      fprintf(output, "@%s\n", symbol);
      fprintf(output, "A=D+M\t\t// D += %s\n", symbol);
      fprintf(output, "D=M\t\t// D = *(%s + D)\n", symbol);
    }
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=D\t\t// *SP = D\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // pop {{{2
  } else if (!strcmp(command, "pop")) {
    if (!strcmp(arg1, "static")) {
      fprintf(output, "@SP\n");
      fprintf(output, "M=M-1\t\t// SP--\n");
      fprintf(output, "A=M\n");
      fprintf(output, "D=M\t\t// D = *SP\n");
      fprintf(output, "@%s.%s\n", fname, arg2);
      fprintf(output, "M=D\t\t// *%s.%s = D\n", fname, arg2);
    } else if (!strcmp(arg1, "pointer")) {
      fprintf(output, "@SP\n");
      fprintf(output, "M=M-1\t\t// SP--\n");
      fprintf(output, "@SP\n");
      fprintf(output, "A=M\n");
      fprintf(output, "D=M\t\t// D = *SP\n");
      fprintf(output, "@%s\n", (c) ? "THAT" : "THIS");
      fprintf(output, "M=D\t\t// THIS/THAT = D\n");
    } else {
      if (!strcmp(arg1, "temp")) {
        fprintf(output, "@%d\n", c);
        fprintf(output, "D=A\t\t// D = %d\n", c);
        fprintf(output, "@5\n");
        fprintf(output, "D=D+A\t\t// D += 5\n");
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
        fprintf(output, "@%d\n", c);
        fprintf(output, "D=A\t\t// D = %d\n", c);
        fprintf(output, "@%s\n", symbol);
        fprintf(output, "D=D+M\t\t// D += %s\n", symbol);
      }
      fprintf(output, "@R13\n");
      fprintf(output, "M=D\t\t// R13 = D\n");
      fprintf(output, "@SP\n");
      fprintf(output, "M=M-1\t\t// SP--\n");
      fprintf(output, "@SP\n");
      fprintf(output, "A=M\n");
      fprintf(output, "D=M\t\t// D = *SP\n");
      fprintf(output, "@R13\n");
      fprintf(output, "A=M\n");
      fprintf(output, "M=D\t\t// *R13 = D\n");
    }
    // add {{{2
  } else if (!strcmp(command, "add")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=D+M\t\t// *SP += D\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // sub {{{2
  } else if (!strcmp(command, "sub")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=M-D\t\t// *SP -= D\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // neg {{{2
  } else if (!strcmp(command, "neg")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=-M\t\t// *SP = -(*SP)\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // eq {{{2
  } else if (!strcmp(command, "eq")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M-D\t\t// D = *SP - D\n");
    fprintf(output, "@EQ_%zu\n", lineNumber);
    fprintf(output, "D;JEQ\t\t// IF (D = 0) GOTO EQ_%zu\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=0\t\t// *SP = false\n");
    fprintf(output, "@CONT_%zu\n", lineNumber);
    fprintf(output, "0;JMP\t\t// GOTO CONT_%zu\n", lineNumber);
    fprintf(output, "(EQ_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=-1\t\t// *SP = true\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // gt {{{2
  } else if (!strcmp(command, "gt")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M-D\t\t// D = *SP - D\n");
    fprintf(output, "@GT_%zu\n", lineNumber);
    fprintf(output, "D;JGT\t\t// IF (D > 0) GOTO GT_%zu\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=0\t\t// *SP = false\n");
    fprintf(output, "@CONT_%zu\n", lineNumber);
    fprintf(output, "0;JMP\t\t// GOTO CONT_%zu\n", lineNumber);
    fprintf(output, "(GT_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=-1\t\t// *SP = true\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // lt {{{2
  } else if (!strcmp(command, "lt")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M-D\t\t// D = *SP - D\n");
    fprintf(output, "@LT_%zu\n", lineNumber);
    fprintf(output, "D;JLT\t\t// IF (D < 0) GOTO LT_%zu\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=0\t\t// *SP = false\n");
    fprintf(output, "@CONT_%zu\n", lineNumber);
    fprintf(output, "0;JMP\t\t// GOTO CONT_%zu\n", lineNumber);
    fprintf(output, "(LT_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=-1\t\t// *SP = true\n");
    fprintf(output, "(CONT_%zu)\n", lineNumber);
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // and {{{2
  } else if (!strcmp(command, "and")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=D&M\t\t// *SP = D & M\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // or {{{2
  } else if (!strcmp(command, "or")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "D=M\t\t// D = *SP\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=D|M\t\t// *SP = D | M\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
    // not {{{2
  } else if (!strcmp(command, "not")) {
    fprintf(output, "@SP\n");
    fprintf(output, "M=M-1\t\t// SP--\n");
    fprintf(output, "A=M\n");
    fprintf(output, "M=!M\t\t// *SP = !M\n");
    fprintf(output, "@SP\n");
    fprintf(output, "M=M+1\t\t// SP++\n");
  } else {
    EXIT_ERROR("Invalid command");
  }
  // }}}2
  return EXIT_SUCCESS;
}
