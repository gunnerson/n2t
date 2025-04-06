// definitions {{{1
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128
#define MAX_TOKEN_LENGTH 16
#define MAX_CONSTANT 32767

#define EXIT_ERROR(t)                                                          \
  do {                                                                         \
    fprintf(stderr, "Error on line %zu: %s\n", lineNumber, t);                 \
    return EXIT_FAILURE;                                                       \
  } while (0)
// print_command {{{1
int print_command(char *command, char *arg1, char *arg2, char *file_name,
                  size_t lineNumber, FILE *output) {
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
      fprintf(output, "@%s.%d\n", file_name, c);
      fprintf(output, "D=M\t\t// D = *%s.%d\n", file_name, c);
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
      fprintf(output, "@%s.%s\n", file_name, arg2);
      fprintf(output, "M=D\t\t// *%s.%s = D\n", file_name, arg2);
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
// main {{{1
int main(int argc, char *argv[]) {
  // handle input {{{2
  FILE *file;
  char *file_name = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1));
  char *full_file_name = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1));
  size_t last_dot_index = 0;
  if (argc >= 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      perror("Error opening file");
      return EXIT_FAILURE;
    }
    size_t last_slash_index = 0;
    for (size_t i = 0; argv[1][i]; i++) {
      if (argv[1][i] == '/' || argv[1][i] == '\\')
        last_slash_index = i + 1;
    }
    for (size_t i = 0; (full_file_name[i] = argv[1][i]); i++) {
      if (full_file_name[i] == '.') {
        last_dot_index = i;
      }
    }
    if (last_dot_index > last_slash_index) {
      full_file_name[last_dot_index] = '\0';
    }
    for (size_t i = 0; (file_name[i++] = full_file_name[last_slash_index++]);)
      ;

  } else {
    file = tmpfile();
    if (file == NULL) {
      perror("Error creating temporary file");
      return EXIT_FAILURE;
    }
    char buffer[256];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
      fwrite(buffer, 1, bytes_read, file);
    }
    rewind(file);
    strcpy(file_name, "stdin");
  }
  // handle output {{{2
  FILE *output;
  if ((argc == 3 && !strcmp(argv[2], "-")) ||
      (argc == 2 && !strcmp(argv[1], "-"))) {
    output = stdout;
  } else {
    char *of = (char *)malloc(sizeof(char) * (strlen(full_file_name) + 5));
    snprintf(of, strlen(full_file_name) + 5, "%s.asm", full_file_name);
    output = fopen(of, "w");
    if (output == NULL) {
      perror("Error opening file");
      return EXIT_FAILURE;
    }
    free(of);
  }
  // parser {{{2
  fprintf(output, "// %s\n\n", file_name);
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
      if (c == ' ' || c == '/' || c == '\r' || c == '\n' || c == '\t') {
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
        } else if (c == '/') {
          break;
        }
      } else {
        token[j++] = c;
      }
    }

    if (*command) {
      fprintf(output, "// %s", command);
      if (*arg1)
        fprintf(output, " %s", arg1);
      if (*arg2)
        fprintf(output, " %s", arg2);
      fprintf(output, "\n");
      if (print_command(command, arg1, arg2, file_name, lineNumber, output)) {
        fclose(file);
        return EXIT_FAILURE;
      }
    }
  }
  // }}}2
  free(file_name);
  free(full_file_name);
  fclose(output);
  fclose(file);
  return EXIT_SUCCESS;
}
