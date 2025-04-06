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

// print_command {{{1
int print_command(char *command, char *arg1, char *arg2, char *file_name) {
  int c = atoi(arg2);
  if (c < 0 || c > MAX_CONSTANT) {
    perror("[!] Address out of bounds!");
    return EXIT_FAILURE;
  }

  // push {{{2
  if (!strcmp(command, "push")) {

    // constant {{{3
    if (!strcmp(arg1, "constant")) {
      printf("@%d\n", c);
      printf("D=A\t\t// D = %d\n", c);

      // local {{{3
    } else if (!strcmp(arg1, "local")) {
      printf("@%d\n", c);
      printf("D=A\t\t// D = %d\n", c);
      printf("@LCL\n");
      printf("A=D+M\t\t// D += LCL\n");
      printf("D=M\t\t// D = *(LCL + D)\n");

      // static {{{3
    } else if (!strcmp(arg1, "static")) {
      printf("@%s.%d\n", file_name, c);
      printf("D=M\t\t// D = *%s.%d\n", file_name, c);

      // pointer {{{3
    } else if (!strcmp(arg1, "pointer")) {
      if (c) {
        printf("@THAT\n");
      } else
        printf("@THIS\n");
      printf("D=M\t\t// D = *(THIS/THAT)\n");

      // temp {{{3
    } else if (!strcmp(arg1, "temp")) {
      printf("@%d\n", c);
      printf("D=A\t\t// D = %d\n", c);
      printf("@5\n");
      printf("A=D+A\n");
      printf("D=M\t\t// D = *(D + 5)\n");
      // }}}3

    } else {
      perror("[!] Invalid segment!");
      return EXIT_FAILURE;
    }
    printf("@SP\n");
    printf("A=M\n");
    printf("M=D\t\t// *SP = D\n");
    printf("@SP\n");
    printf("M=M+1\t\t// SP++\n");

    // pop {{{2
  } else if (!strcmp(command, "pop")) {

    // static {{{3
    if (!strcmp(arg1, "static")) {
      printf("@SP\n");
      printf("M=M-1\t\t// SP--\n");
      printf("A=M\n");
      printf("D=M\t\t// D = *SP\n");
      printf("@%s.%s\n", file_name, arg2);
      printf("M=D\t\t// *%s.%s = D\n", file_name, arg2);

      // local {{{3
    } else if (!strcmp(arg1, "local")) {
      printf("@%d\n", c);
      printf("D=A\t\t// D = %d\n", c);
      printf("@LCL\n");
      printf("D=D+M\t\t// D += LCL\n");
      printf("@R13\n");
      printf("M=D\t\t// R13 = D\n");
      printf("@SP\n");
      printf("M=M-1\t\t// SP--\n");
      printf("@SP\n");
      printf("A=M\n");
      printf("D=M\t\t// D = *SP\n");
      printf("@R13\n");
      printf("A=M\n");
      printf("M=D\t\t// *R13 = D\n");

      // pointer {{{3
    } else if (!strcmp(arg1, "pointer")) {
      printf("@SP\n");
      printf("M=M-1\t\t// SP--\n");
      printf("@SP\n");
      printf("A=M\n");
      printf("D=M\t\t// D = *SP\n");
      if (c) {
        printf("@THAT\n");
      } else
        printf("@THIS\n");
      printf("M=D\t\t// *(THIS/THAT) = D\n");

      // temp {{{3
    } else if (!strcmp(arg1, "temp")) {
      printf("@%d\n", c);
      printf("D=A\t\t// D = %d\n", c);
      printf("@5\n");
      printf("D=D+A\t\t// D += 5\n");
      printf("@R13\n");
      printf("M=D\t\t// R13 = D\n");
      printf("@SP\n");
      printf("M=M-1\t\t// SP--\n");
      printf("@SP\n");
      printf("A=M\n");
      printf("D=M\t\t// D = *SP\n");
      printf("@R13\n");
      printf("A=M\n");
      printf("M=D\t\t// *R13 = D\n");
      // }}}3

    } else {
      perror("[!] Invalid segment!");
      return EXIT_FAILURE;
    }

    // add {{{2
  } else if (!strcmp(command, "add")) {
    printf("@SP\n");
    printf("M=M-1\t\t// SP--\n");
    printf("A=M\n");
    printf("D=M\t\t// D = *SP\n");
    printf("@SP\n");
    printf("M=M-1\t\t// SP--\n");
    printf("A=M\n");
    printf("M=D+M\t\t// *SP += D\n");
    printf("@SP\n");
    printf("M=M+1\t\t// SP++\n");

    // sub {{{2
  } else if (!strcmp(command, "sub")) {
    printf("@SP\n");
    printf("M=M-1\t\t// SP--\n");
    printf("A=M\n");
    printf("D=M\t\t// D = *SP\n");
    printf("@SP\n");
    printf("M=M-1\t\t// SP--\n");
    printf("A=M\n");
    printf("M=M-D\t\t// *SP -= D\n");
    printf("@SP\n");
    printf("M=M+1\t\t// SP++\n");

  } else if (!strcmp(command, "neg")) {

  } else if (!strcmp(command, "eq")) {

  } else if (!strcmp(command, "gt")) {

  } else if (!strcmp(command, "lt")) {

  } else if (!strcmp(command, "and")) {

  } else if (!strcmp(command, "or")) {

  } else if (!strcmp(command, "not")) {
  }

  return EXIT_SUCCESS;
}

// main {{{1
int main(int argc, char *argv[]) {
  // handle file {{{2
  FILE *file;
  char file_name[MAX_LINE_LENGTH] = {0};
  if (argc >= 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      perror("Error opening file");
      return EXIT_FAILURE;
    }
    for (size_t i = 0; argv[1][i] != '.'; i++) {
      file_name[i] = argv[1][i];
    }
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
    strcpy(file_name, "tmp");
  }

  // parser {{{2
  char line[MAX_LINE_LENGTH] = {0};
  while (fgets(line, MAX_LINE_LENGTH, file)) {
    // get tokens {{{3
    char token[MAX_LINE_LENGTH] = {0};
    char command[MAX_TOKEN_LENGTH] = {0};
    char arg1[MAX_TOKEN_LENGTH] = {0};
    char arg2[MAX_TOKEN_LENGTH] = {0};
    size_t j = 0;
    for (size_t i = 0; i < strlen(line); i++) {
      char c = line[i];

      if (c == ' ' || c == '/' || c == '\r' || c == '\n') {
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

    // translate command {{{3
    if (*command) {
      printf("// %s", command);
      if (*arg1)
        printf(" %s", arg1);
      if (*arg2)
        printf(" %s", arg2);
      printf("\n");
    }
    if (print_command(command, arg1, arg2, file_name)) {
      fclose(file);
      return EXIT_FAILURE;
    }
  }

  fclose(file);
  return EXIT_SUCCESS;
}
