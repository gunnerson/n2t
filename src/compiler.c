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
// definitions {{{2
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

typedef union {
  Keyword keyword;
  char symbol;
  unsigned intVal;
  char *strVal;
} TokenData;

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
// data structures {{{2
typedef struct Token {
  TokenType type;
  TokenData data;
  size_t lineN;
  size_t lineP;
  struct Token *next;
  struct Token *nextSibling;
  struct Token *children;
} Token;

typedef struct {
  Token *head;
  Token *tail;
} TokenList;

void add_token(TokenList *t, TokenType const type, TokenData const data,
               size_t lineN, size_t lineP) {
  Token *new = malloc(sizeof(Token));
  if (new) {
    new->type = type;
    switch (type) {
    case KEYWORD:
      new->data.keyword = data.keyword;
      break;
    case SYMBOL:
      new->data.symbol = data.symbol;
      break;
    case INT_CONST:
      new->data.intVal = data.intVal;
      break;
    default:
      new->data.strVal = strdup(data.strVal);
      if (new->data.strVal == NULL) {
        perror("Failed to allocate memory for a token");
        free(new);
        return;
      }
    }
    new->lineN = lineN;
    new->lineP = lineP;
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
    switch (cur->type) {
    case KEYWORD:
      printf("%d\t%zu:%zu\t%d\n", cur->type, cur->lineN, cur->lineP,
             cur->data.keyword);
      break;
    case SYMBOL:
      printf("%d\t%zu:%zu\t%c\n", cur->type, cur->lineN, cur->lineP,
             cur->data.symbol);
      break;
    case INT_CONST:
      printf("%d\t%zu:%zu\t%d\n", cur->type, cur->lineN, cur->lineP,
             cur->data.intVal);
      break;
    default:
      printf("%d\t%zu:%zu\t%s\n", cur->type, cur->lineN, cur->lineP,
             cur->data.strVal);
    }
    cur = cur->next;
  }
}

void token_list_del(TokenList *t) {
  Token *next, *cur = t->head;
  while (cur) {
    if (cur->type == IDENTIFIER || cur->type == STR_CONST)
      free(cur->data.strVal);
    next = cur->next;
    free(cur);
    cur = next;
  }
  free(t);
}

int parse_token(TokenList *t, char *str, unsigned lineN, unsigned lineP) {
  Keyword kw = keyword_from_str(str);
  TokenType type;
  TokenData data;
  if (kw != -1) {
    type = KEYWORD;
    data.keyword = kw;
  } else {
    int intVal = is_intConst(str);
    if (intVal != -1) {
      type = INT_CONST;
      data.intVal = intVal;
    } else if (is_identifier(str)) {
      type = IDENTIFIER;
      data.strVal = str;
    } else
      return EXIT_FAILURE;
  }
  add_token(t, type, data, lineN, lineP);
  return EXIT_SUCCESS;
}

// tokenize_file {{{2
TokenList *tokenize_file(FILE *file) {
  TokenList *tl = malloc(sizeof(TokenList));
  if (tl == NULL) {
    perror("Failed to allocate memory for a token list");
    return NULL;
  }
  tl->head = NULL;
  tl->tail = NULL;
  char line[MAX_LINE_LENGTH];
  TokenData data;
  bool isMultComment = false; // Inside multi-line comment
  for (size_t lineNumber = 1; fgets(line, sizeof(line), file); lineNumber++) {
    char *c = line;
    char token[sizeof(line)] = "";
    bool isStr = false;   // Met open quote, start collecting string constant
    bool isSlash = false; // Met slash, check if it's 'division' or comment

    for (size_t i = 1, j = 0; i < MAX_LINE_LENGTH; c++, i++) {
      // Handle multi-line comments
      if (isMultComment) {
        if (isspace(*c))
          continue;
        if (*c == '*')
          break;
        isMultComment = false;
      }
      // Handle string constants
      if (isStr) {
        if (*c == '"') {
          token[j] = '\0';
          data.strVal = token;
          add_token(tl, STR_CONST, data, lineNumber, i - strlen(token));
          *token = '\0';
          j = 0;
          isStr = false;
        } else {
          token[j++] = *c;
        }
        // Handle 'slash' character
      } else if (isSlash) {
        if (*c == '/')
          break;
        else if (*c == '*') {
          isMultComment = true;
          break;
        } else {
          data.symbol = *--c;
          add_token(tl, SYMBOL, data, lineNumber, --i);
          isSlash = false;
        }

      } else {
        if (isspace(*c) || is_symbol(*c) || *c == '\0') {
          if (*token) {
            token[j] = '\0';
            if (parse_token(tl, token, lineNumber, i - strlen(token))) {
              fprintf(stderr, "%zu:%zu Parsing error. Bad token '%s'\n",
                      lineNumber, i, token);
              errno = PARSING_ERROR;
              break;
            }
            *token = '\0';
            j = 0;
          }
          if (is_symbol(*c)) {
            if (*c == '/') {
              isSlash = true;
            } else {
              data.symbol = *c;
              add_token(tl, SYMBOL, data, lineNumber, i);
            }
          }
          if (*c == '\0' || *c == '\n')
            break;
        } else if (*c == '"') {
          isStr = true;
        } else if (*c) {
          token[j++] = *c;
        }
      }
    }
    if (errno)
      break;
  }
  // token_list_dump(tl);
  return tl;
}
// parse_tokens {{{1
void parse_tokens(TokenList *tl) {
  Token *cur = tl->head;
  for (; cur; cur = cur->next) {
    if (cur->type == KEYWORD || cur->data.keyword == CLASS)
  }
}
// handle_file {{{1
void handle_file(FILE *in, FILE *out) {
  TokenList *tl = tokenize_file(in);
  parse_tokens(tl);
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
  errno = 0;

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
          handle_file(file, ofile);
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
                strcpy(dot, ".vm");
                snprintf(file_path, PATH_MAX, "%s%c%s", path, SLASH,
                         entry->d_name);
                FILE *ofile = fopen(file_path, "w");
                if (ofile) {
                  handle_file(file, ofile);
                  fclose(ofile);
                } else
                  perror("Error opening/creating file");
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
