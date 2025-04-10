// definitions {{{1
#include <ctype.h>
#include <dirent.h>
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
#define DT_REG 8

#define EXIT_ERROR(t)                                                          \
  do {                                                                         \
    fprintf(stderr, "Error on line %zu: %s\n", lineNumber, t);                 \
    return EXIT_FAILURE;                                                       \
  } while (0)

void parse_file(FILE *file, FILE *ofile, char const *fname);
int write_command(char const *command, char const *arg1, char const *arg2,
                  char const *fname, size_t const lineNumber, FILE *output);

// main {{{1
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct stat path_stat;
  if (stat(argv[1], &path_stat) == -1) {
    perror("Errot getting file attributes");
    return EXIT_FAILURE;
  }

  if (S_ISREG(path_stat.st_mode)) {
    FILE *file = fopen(argv[1], "r");
    if (file) {
      char *ofname = malloc(strlen(argv[1]) + 5);
      char *fname = malloc(strlen(argv[1]));
      if (ofname && fname) {
        strcpy(ofname, argv[1]);
        char *dot = strrchr(ofname, '.');
        if (dot && !strcmp(dot, ".vm")) {
          *dot = '\0';
          char *slash = strrchr(ofname, '/');
          (slash) ? strcpy(fname, slash + 1) : strcpy(fname, ofname);
          strcpy(dot, ".asm");
          FILE *ofile = fopen(ofname, "w");
          if (ofile) {
            parse_file(file, ofile, fname);
            fclose(ofile);
          } else
            fprintf(stderr, "Error creating output file: %s\n", ofname);
        } else
          perror("Invalid file path");
      } else
        perror("Failed to allocate memory");
      if (ofname)
        free(ofname);
      if (fname)
        free(fname);
    } else {
      fprintf(stderr, "Error opening file %s\n", argv[1]);
    }
    fclose(file);

  } else if (S_ISDIR(path_stat.st_mode)) {
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
      perror("Error opening directory");
      return EXIT_FAILURE;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
      if (entry->d_type == DT_REG) {
        char fname[256];
        strcpy(fname, entry->d_name);
        char *dot = strrchr(fname, '.');
        if (dot && !strcmp(dot, ".vm")) {
          *dot = '\0';
          char *file_path = malloc(strlen(argv[1]) + strlen(entry->d_name) + 5);
          if (file_path) {
            sprintf(file_path,
                    (argv[1][strlen(argv[1]) - 1] == '/') ? "%s%s" : "%s/%s",
                    argv[1], entry->d_name);
            FILE *file = fopen(file_path, "r");
            if (file) {
              sprintf(file_path,
                      (argv[1][strlen(argv[1]) - 1] == '/') ? "%s%s.asm"
                                                            : "%s/%s.asm",
                      argv[1], fname);
              FILE *ofile = fopen(file_path, "w");
              if (ofile) {
                parse_file(file, ofile, fname);
                fclose(ofile);
              } else
                fprintf(stderr, "Error creating output file: %s\n", file_path);
              fclose(file);
            } else {
              fprintf(stderr, "Error opening file %s\n", entry->d_name);
            }
            free(file_path);
          } else {
            fprintf(stderr, "Memory allocation error while opening file %s\n",
                    entry->d_name);
          }
        }
      }
    }
    closedir(dir);
  } else {
    perror("Error opening files");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
// parse_file {{{1
void parse_file(FILE *file, FILE *ofile, char const *fname) {
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
