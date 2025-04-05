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

// push_segment {{{1
int push_segment(char *arg1, char *arg2) {
  int c = atoi(arg2);
  if (c < 0 || c > MAX_CONSTANT) {
    perror("[!] Address out of bounds!");
    return EXIT_FAILURE;
  }

  if (!strcmp(arg1, "constant")) {
    printf("@%d\n", c);
    printf("D=A\t\t// D = %d\n", c);
  } else if (!strcmp(arg1, "argument")) {

  } else if (!strcmp(arg1, "local")) {
    printf("@%d\n", c);
    printf("D=A\t\t// D = %d\n", c);
    printf("@LCL\n");
    printf("A=D+M\t\t// D += LCL\n");
    printf("D=M\t\t// D = *(LCL + D)\n");

  } else if (!strcmp(arg1, "static")) {
  } else if (!strcmp(arg1, "this")) {
  } else if (!strcmp(arg1, "that")) {
  } else if (!strcmp(arg1, "pointer")) {
  } else if (!strcmp(arg1, "temp")) {

  } else {
    perror("[!] Invalid segment!");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// pop_segment {{{1
int pop_segment(char *arg1, char *arg2) {
  int c = atoi(arg2);
  if (c < 0 || c > MAX_CONSTANT) {
    perror("[!] Address out of bounds!");
    return EXIT_FAILURE;
  }

  if (!strcmp(arg1, "local")) {
    printf("@%d\n", c);
    printf("D=A\t\t// D = %d\n", c);
    printf("@LCL\n");
    printf("D=D+M\t\t// D += LCL\n");
    printf("@R13\n");
    printf("M=D\t\t// R13 = D\n");

  } else if (!strcmp(arg1, "static")) {
  } else if (!strcmp(arg1, "this")) {
  } else if (!strcmp(arg1, "that")) {
  } else if (!strcmp(arg1, "pointer")) {
  } else if (!strcmp(arg1, "temp")) {

  } else {
    perror("[!] Invalid segment!");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// print_command {{{1
int print_command(char *command, char *arg1, char *arg2) {
  if (!strcmp(command, "push")) {
    if (push_segment(arg1, arg2))
      return EXIT_FAILURE;
    printf("@SP\n");
    printf("A=M\n");
    printf("M=D\t\t// *SP = D\n");
    printf("@SP\n");
    printf("M=M+1\t\t// SP++\n");

  } else if (!strcmp(command, "pop")) {
    if (pop_segment(arg1, arg2))
      return EXIT_FAILURE;
    printf("@SP\n");
    printf("M=M-1\t\t// SP--\n");
    printf("@SP\n");
    printf("A=M\n");
    printf("D=M\t\t// D = *SP\n");
    printf("@R13\n");
    printf("A=M\n");
    printf("M=D\t\t// *R13 = D\n");

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

  } else if (!strcmp(command, "sub")) {
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
  if (argc >= 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      perror("Error opening file");
      return EXIT_FAILURE;
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
    if (print_command(command, arg1, arg2)) {
      perror("[!] Translation error!");
      fclose(file);
      return EXIT_FAILURE;
    }
  }

  fclose(file);
  return EXIT_SUCCESS;
}
