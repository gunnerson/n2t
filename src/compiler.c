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

#define MAX_LINE_LENGTH 256
#define MAX_TOKEN_LENGTH 32
#define MAX_CONSTANT 32767
#define MAX_FILE_NAME 256
#define DT_REG 8
#define DT_UNKNOWN 0
#define SLASH '/'
#define PARSING_ERROR 1001

#define STACK_ADDRESS 256UCLASS, METHOD, FUNCTION,

extern char *realpath(const char *restrict path, char *restrict resolved_path);

// tokenizer {{{1
// enums {{{2
typedef enum {
  KEYWORD,
  SYMBOL,
  IDENTIFIER,
  INT_CONST,
  STR_CONST,
} TokenType;

typedef enum {
  CONSTRUCTOR,
  INT,
  BOOLEAN,
  CHAR,
  VOID,
  VAR,
  STATIC,
  FIELD,
  LET,
  DO,
  IF,
  ELSE,
  WHILE,
  RETURN,
  TRUE,
  FALSE,
  NUL,
  THIS,
  keyword_num,
} Keyword;

char const *const keywords[keyword_num] = {[CONSTRUCTOR] = "constructor",
                                           [INT] = "int",
                                           [BOOLEAN] = "boolean",
                                           [CHAR] = "char",
                                           [VOID] = "void",
                                           [VAR] = "var",
                                           [STATIC] = "static",
                                           [FIELD] = "field",
                                           [LET] = "let",
                                           [DO] = "do",
                                           [IF] = "if",
                                           [ELSE] = "else",
                                           [WHILE] = "while",
                                           [RETURN] = "return",
                                           [TRUE] = "true",
                                           [FALSE] = "false",
                                           [NUL] = "null",
                                           [THIS] = "this"};

// aux functions {{{2
Keyword keyword_from_str(char const *str) {
  for (int i = 0; i < keyword_num; i++) {
    if (!strcmp(str, keywords[i])) {
      return (Keyword)i;
    }
  }
  return -1;
}

bool is_symbol(char const c) {
  return (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' ||
          c == ']' || c == '.' || c == ',' || c == ';' || c == '+' ||
          c == '-' || c == '*' || c == '/' || c == '&' || c == '|' ||
          c == '<' || c == '>' || c == '=' || c == '~');
}

int is_intConst(char const *str) {
  if (!strcmp(str, "0"))
    return 0;
  int val = atoi(str);
  if (val > 0 && val <= MAX_CONSTANT)
    return val;
  return -1;
}

bool is_strConst(char const *str) {
  return (*str == '"' && *(str + strlen(str) - 1) == '"');
}

bool is_identifier(char const *str) {
  while (*str) {
    if (!(*str >= 'a' && *str <= 'z') && !(*str >= 'A' && *str <= 'Z') &&
        !(*str >= '0' && *str <= '9') && *str != '_')
      return false;
    str++;
  }
  if (isdigit(str[0]))
    return false;
  return true;
}
// tokens DS {{{2
typedef struct Token {
  TokenType type;
  Keyword keyword;
  char symbol;
  char *identifier;
  unsigned intVal;
  char *strVal;
  struct Token *next;
  struct Token *nextSibling;
  struct Token *children;
} Token;

typedef struct {
  Token *head;
  Token *tail;
} TokenList;

void add_token(TokenList *t, TokenType const type, Keyword const keyword,
               char const symbol, char const *identifier, unsigned const intVal,
               char const *strVal) {
  Token *new = malloc(sizeof(Token));
  if (new) {
    new->type = type;
    new->keyword = keyword;
    new->symbol = symbol;
    if (identifier) {
      new->identifier = strdup(identifier);
      if (!new->identifier) {
        perror("Failed to allocate memory for a token");
        free(new);
        return;
      }
    } else
      new->identifier = NULL;
    new->intVal = intVal;
    if (strVal) {
      new->strVal = strdup(strVal);
      if (!new->strVal) {
        perror("Failed to allocate memory for a token");
        free(new);
        return;
      }
    } else
      new->strVal = NULL;
    new->next = NULL;
    new->nextSibling = NULL;
    new->children = NULL;
    if (t->tail)
      t->tail->next = new;
    else
      t->head = new;
    t->tail = new;
  } else
    perror("Failed to allocate memory for a token");
}

void token_list_dump(TokenList *t) {
  Token *cur = t->head;
  while (cur) {
    printf("%d\t%d\t%c\t%s\t\t\t%u\t%s\n", cur->type, cur->keyword, cur->symbol,
           cur->identifier, cur->intVal, cur->strVal);
    cur = cur->next;
  }
}

void token_list_del(TokenList *t) {
  Token *next, *cur = t->head;
  while (cur) {
    if (cur->identifier)
      free(cur->identifier);
    else if (cur->strVal)
      free(cur->strVal);
    next = cur->next;
    free(cur);
    cur = next;
  }
  free(t);
}

int parse_token(TokenList *t, char const *str) {
  Keyword kw = keyword_from_str(str);
  if (kw != -1) {
    add_token(t, KEYWORD, kw, 0, NULL, 0, NULL);
  } else {
    int intVal = is_intConst(str);
    if (intVal != -1)
      add_token(t, INT_CONST, 0, 0, NULL, (unsigned)intVal, NULL);
    else if (is_strConst(str))
      add_token(t, STR_CONST, 0, 0, NULL, 0, str);
    else if (is_identifier(str))
      add_token(t, IDENTIFIER, 0, 0, str, 0, NULL);
    else
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// parse_file {{{1
void parse_file(FILE *file) {
  TokenList *tl = malloc(sizeof(TokenList));
  if (tl == NULL) {
    perror("Failed to allocate memory for a token list");
    return;
  }
  tl->head = NULL;
  tl->tail = NULL;
  char line[MAX_LINE_LENGTH];
  for (size_t lineNumber = 1; fgets(line, sizeof(line), file); lineNumber++) {
    char *c = line;
    char token[sizeof(line)] = "";

    for (size_t i = 0, j = 0; i < MAX_LINE_LENGTH; c++) {
      if (isspace(*c) || is_symbol(*c) || *c == '\0') {
        if (*token) {
          token[j] = '\0';
          if (parse_token(tl, token)) {
            fprintf(stderr, "Parsing error on line %zu:%zu (bad token)\n",
                    lineNumber, i);
            errno = PARSING_ERROR;
            break;
          }
          *token = '\0';
          j = 0;
        }
        if (is_symbol(*c))
          add_token(tl, SYMBOL, 0, *c, NULL, 0, NULL);
        if (*c == '\0' || *c == '\n')
          break;
      } else if (*c) {
        token[j++] = *c;
      }
    }
    // if (errno)
    //   break;
  }
  token_list_dump(tl);
  token_list_del(tl);
}
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
          parse_file(file);
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
      while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
          char *file_path = malloc(PATH_MAX);
          if (file_path) {
            char *dot = strrchr(entry->d_name, '.');
            if (dot && !strcmp(dot, ".jack")) {
              snprintf(file_path, PATH_MAX, "%s%c%s", path, SLASH,
                       entry->d_name);
              FILE *file = fopen(file_path, "r");
              if (file) {
                parse_file(file);
                fclose(file);
              } else
                perror("Error opening file");
            }
            free(file_path);
          } else
            perror("Error allocating memory");
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
