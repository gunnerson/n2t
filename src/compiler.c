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
#define INITIAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define STACK_ADDRESS 256UCLASS, METHOD, FUNCTION,

extern char *realpath(const char *restrict path, char *restrict resolved_path);
FILE *out;

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
  CLASS,
  CONSTRUCTOR,
  FUNCTION,
  METHOD,
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

char const *const keywords[keyword_num] = {[CLASS] = "class",
                                           [CONSTRUCTOR] = "constructor",
                                           [FUNCTION] = "function",
                                           [METHOD] = "method",
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
  int intVal;
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
// token list {{{2
typedef struct Token {
  TokenType type;
  TokenData data;
  size_t lineN;
  size_t lineP;
  struct Token *next;
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
    new->data.keyword = -1;
    new->data.symbol = '\0';
    new->data.intVal = -1;
    new->data.strVal = "";
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
    bool isStr = false; // Met open quote, start collecting string constant

    for (size_t i = 1, j = 0; i < strlen(line); c++, i++) {
      // Handle multi-line comments
      if (isMultComment) {
        if (*c == '*' && *(c + 1) == '/') {
          isMultComment = false;
          break;
        }
        continue;
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
            if (*c == '/' && *(c + 1) == '*' && *(c + 2) == '*') {
              isMultComment = true;
            } else if (*c == '/' && *(c + 1) == '/') {
              break;
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
  return tl;
}
// symbol-table {{{1
// declarations {{{2
typedef enum {
  FIELD_,
  STATIC_,
  LOCAL_,
  ARGUMENT_,
  sk_num,
} SymbolKind;

char const *const symbol_kind[sk_num] = {
    [FIELD_] = "field",
    [STATIC_] = "static",
    [LOCAL_] = "local",
    [ARGUMENT_] = "argument",
};

typedef struct {
  const char *name;
  const char *type;
  SymbolKind kind;
  size_t idx;
} Symbol;
typedef struct {
  Symbol *entries;
  size_t capacity;
  size_t length;
  size_t field_idx;
  size_t static_idx;
  size_t local_idx;
  size_t argument_idx;
} SymbolTable;
// st_new {{{2
SymbolTable *st_new(void) {
  SymbolTable *self = malloc(sizeof(SymbolTable));
  if (self == NULL) {
    perror("Failed to allocate memory!");
    return NULL;
  }
  self->length = 0;
  self->field_idx = 0;
  self->static_idx = 0;
  self->local_idx = 0;
  self->argument_idx = 0;
  self->capacity = INITIAL_CAPACITY;
  self->entries = calloc(self->capacity, sizeof(Symbol));
  if (self->entries == NULL) {
    perror("Failed to allocate memory!");
    free(self);
    return NULL;
  }
  return self;
}
// st_del {{{2
void *st_del(SymbolTable *self) {
  for (size_t i = 0; i < self->capacity; i++) {
    free((void *)self->entries[i].name);
    free((void *)self->entries[i].type);
  }
  free(self->entries);
  free(self);
  return NULL;
}
// st_hash {{{2
static uint64_t st_hash(const char *key) {
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}
// st_get {{{2
Symbol *st_get(SymbolTable *self, const char *name) {
  uint64_t hash = st_hash(name);
  size_t index = (size_t)(hash & (uint64_t)(self->capacity - 1));
  while (self->entries[index].name) {
    if (!strcmp(name, self->entries[index].name)) {
      return &self->entries[index];
    }
    index++;
    if (index >= self->capacity) {
      index = 0;
    }
  }
  return NULL;
}
// st_set_entry {{{2
void st_set_entry(Symbol *entries, size_t capacity, const char *name,
                  const char *type, SymbolKind kind, unsigned idx,
                  size_t *length_ptr) {
  uint64_t hash = st_hash(name);
  size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
  while (entries[index].name) {
    if (!strcmp(name, entries[index].name)) {
      return;
    }
    index++;
    if (index >= capacity) {
      index = 0;
    }
  }
  if (length_ptr != NULL) {
    name = strdup(name);
    if (name == NULL) {
      perror("Failed to allocate memory!");
      return;
    }
    (*length_ptr)++;
  }
  type = strdup(type);
  if (type == NULL) {
    perror("Failed to allocate memory!");
    free((char *)name);
    return;
  }
  entries[index].name = (char *)name;
  entries[index].type = (char *)type;
  entries[index].kind = kind;
  entries[index].idx = idx;
  return;
}
// st_expand {{{2
static bool st_expand(SymbolTable *self) {
  size_t new_capacity = self->capacity * 2;
  if (new_capacity < self->capacity) {
    return false;
  }
  Symbol *new_entries = calloc(new_capacity, sizeof(Symbol));
  if (new_entries == NULL) {
    perror("Failed to allocate memory!");
    return false;
  }
  for (size_t i = 0; i < self->capacity; i++) {
    Symbol entry = self->entries[i];
    if (entry.name != NULL) {
      st_set_entry(new_entries, new_capacity, entry.name, entry.type,
                   entry.kind, entry.idx, NULL);
    }
  }
  free(self->entries);
  self->entries = new_entries;
  self->capacity = new_capacity;
  return true;
}
// st_set {{{2
void st_set(SymbolTable *self, const char *name, const char *type,
            SymbolKind kind) {
  unsigned idx;
  if (self->length >= self->capacity / 2) {
    if (!st_expand(self)) {
      return;
    }
  }
  switch (kind) {
  case FIELD_:
    idx = self->field_idx;
    self->field_idx++;
    break;
  case STATIC_:
    idx = self->static_idx;
    self->static_idx++;
    break;
  case LOCAL_:
    idx = self->local_idx;
    self->local_idx++;
    break;
  case ARGUMENT_:
    idx = self->argument_idx;
    self->argument_idx++;
    break;
  default:
    idx = 0;
  }

  st_set_entry(self->entries, self->capacity, name, type, kind, idx,
               &self->length);
  return;
}
// compilation engine {{{1
// declarations {{{2
int compExpressionList();
void compTerm();
int compExpression();
void compReturn();
void compWhile();
void compIf();
void compLet();
void compSubroutineCall();
void compDo();
void compStatements();
int compVarDec();
void compParameterList();
void compSubroutine();
void compClassVarDec();
void compClass();
char className[MAX_LINE_LENGTH];
SymbolTable *cst;
SymbolTable *sst;
Token *t;
size_t gotoDepth = 0;
size_t gotoInc = 0;
// compExpressionList {{{2
int compExpressionList() {
  // (expression (',' expression)* )?
  int nArgs = 0;

  nArgs += compExpression();

  while (t->type == SYMBOL && t->data.symbol == ',') {
    t = t->next;
    nArgs += compExpression();
  }

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid expression list\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  return nArgs;
}
// compTerm {{{2
void compTerm() {
  /*   integerConstant | stringConstant | keywordConstant |
   * varName | varName '[' expression ']' | subroutineCall |
   * '(' expression ')' | unaryOp term */
  Symbol *symbol;
  if (t->type == INT_CONST) {
    fprintf(out, "\tpush constant %d\n", t->data.intVal);
    t = t->next;

  } else if (t->type == STR_CONST) {
    // String constants are created using the OS constructor String.new(length).
    // String assignments like x="cc...c" are handled using a series of calls to
    // the OS routine String.appendChar(nextChar).
    fprintf(out, "\tpush constant %lu\n", strlen(t->data.strVal));
    fprintf(out, "\tcall String.new 1\n");
    for (size_t i = 0; i < strlen(t->data.strVal); i++) {
      fprintf(out, "\tpush constant %d\n", t->data.strVal[i]);
      fprintf(out, "\tcall String.appendChar 2\n");
    }
    t = t->next;

  } else if (t->type == KEYWORD && t->data.keyword == TRUE) {
    fprintf(out, "\tpush constant 1\n\tneg\n");
    t = t->next;

  } else if (t->type == KEYWORD &&
             (t->data.keyword == FALSE || t->data.keyword == NUL)) {
    fprintf(out, "\tpush constant 0\n");
    t = t->next;

  } else if (t->type == KEYWORD && t->data.keyword == THIS) {
    fprintf(out, "\tpush pointer 0\n");
    t = t->next;

  } else if (t->type == IDENTIFIER) {

    if (t->next->type == SYMBOL &&
        (t->next->data.symbol == '(' || t->next->data.symbol == '.')) {
      compSubroutineCall();

    } else {
      symbol = st_get(sst, t->data.strVal);
      if (!symbol)
        symbol = st_get(cst, t->data.strVal);
      if (symbol) {
        t = t->next;
        if (t->type == SYMBOL && t->data.symbol == '[') {
          t = t->next;

          compExpression();

          if (t->type == SYMBOL && t->data.symbol == ']') {
            fprintf(out, "\tpush %s %zu\n", symbol_kind[symbol->kind],
                    symbol->idx);
            fprintf(out, "\tadd\n");
            fprintf(out, "\tpop pointer 1\n");
            fprintf(out, "\tpush that 0\n");
            t = t->next;

          } else
            errno = PARSING_ERROR;
        } else {

          switch (symbol->kind) {
          case LOCAL_:
            fprintf(out, "\tpush local %zu\n", symbol->idx);
            break;
          case ARGUMENT_:
            fprintf(out, "\tpush argument %zu\n", symbol->idx);
            break;
          case FIELD_:
            fprintf(out, "\tpush this %zu\n", symbol->idx);
            break;
          case STATIC_:
            fprintf(out, "\tpush static %zu\n", symbol->idx);
            break;
          default:
            break;
          }
        }
      }
    }

  } else if (t->type == SYMBOL && t->data.symbol == '(') {
    t = t->next;

    compExpression();

    if (t->type == SYMBOL && t->data.symbol == ')') {
      t = t->next;

    } else
      errno = PARSING_ERROR;

  } else if (t->type == SYMBOL && t->data.symbol == '-') {
    t = t->next;

    compTerm();

    fprintf(out, "\tneg\n");

  } else if (t->type == SYMBOL && t->data.symbol == '~') {
    t = t->next;

    compTerm();

    fprintf(out, "\tnot\n");
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid term\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compExpression {{{2
int compExpression() {
  // term (op term)*
  Token *op;
  if ((t->type == INT_CONST) || (t->type == STR_CONST) ||
      ((t->type == KEYWORD &&
        (t->data.keyword == TRUE || t->data.keyword == FALSE ||
         t->data.keyword == NUL || t->data.keyword == THIS))) ||
      (t->type == IDENTIFIER) || (t->type == SYMBOL && t->data.symbol == '(') ||
      (t->type == SYMBOL && (t->data.symbol == '-' || t->data.symbol == '~'))) {

    compTerm();

    while (t->type == SYMBOL &&
           (t->data.symbol == '+' || t->data.symbol == '-' ||
            t->data.symbol == '*' || t->data.symbol == '/' ||
            t->data.symbol == '&' || t->data.symbol == '|' ||
            t->data.symbol == '<' || t->data.symbol == '>' ||
            t->data.symbol == '=')) {
      op = t;
      t = t->next;

      compTerm();

      switch (op->data.symbol) {
      case '+':
        fprintf(out, "\tadd\n");
        break;
      case '-':
        fprintf(out, "\tsub\n");
        break;
      case '*':
        fprintf(out, "\tcall Math.multiply 2\n");
        break;
      case '/':
        fprintf(out, "\tcall Math.divide 2\n");
        break;
      case '>':
        fprintf(out, "\tgt\n");
        break;
      case '<':
        fprintf(out, "\tlt\n");
        break;
      case '=':
        fprintf(out, "\teq\n");
        break;
      case '&':
        fprintf(out, "\tand\n");
        break;
      case '|':
        fprintf(out, "\tor\n");
        break;
      }
    }
    return 1;
  }

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid expression\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  return 0;
}
// compReturn {{{2
void compReturn() {
  // 'return' expression? ';'

  if (t->type == SYMBOL && t->data.symbol == ';') {
    fprintf(out, "\tpush constant 0\n");
    fprintf(out, "\treturn\n");
    t = t->next;

  } else if (t->type == KEYWORD && t->data.keyword == THIS) {
    fprintf(out, "\tpush pointer 0\n");
    fprintf(out, "\treturn\n");
    t = t->next;

    if (t->type == SYMBOL && t->data.symbol == ';') {
      t = t->next;

    } else
      errno = PARSING_ERROR;

  } else {
    compExpression();

    if (t->type == SYMBOL && t->data.symbol == ';') {
      fprintf(out, "\treturn\n");
      t = t->next;

    } else
      errno = PARSING_ERROR;
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid return statement\n",
            t->lineN, t->lineP);
    errno = 0;
  }
}
// compWhile {{{2
void compWhile() {
  // 'while' '(' expression ')' '{' statements '}'
  size_t i = gotoInc++;
  gotoDepth++;
  if (t->type == SYMBOL && t->data.symbol == '(') {
    fprintf(out, "label %s_%zu_%zu_0\n", className, i, gotoDepth);
    t = t->next;

    compExpression();

    fprintf(out, "\tnot\n\tif-goto %s_%zu_%zu_1\n", className, i, gotoDepth);

    if (t->type == SYMBOL && t->data.symbol == ')') {
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        t = t->next;

        compStatements();

        if (t->type == SYMBOL && t->data.symbol == '}') {
          fprintf(out, "\tgoto %s_%zu_%zu_0\n", className, i, gotoDepth);
          fprintf(out, "label %s_%zu_%zu_1\n", className, i, gotoDepth);
          t = t->next;

        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  gotoDepth--;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid while statement\n",
            t->lineN, t->lineP);
    errno = 0;
  }
}
// compIf {{{2
void compIf() {
  /*   'if' '(' expression ')' '{' statements '}'
   * ('else' '{' statements '}')? */
  size_t i = gotoInc++;
  gotoDepth++;
  if (t->type == SYMBOL && t->data.symbol == '(') {
    t = t->next;

    compExpression();
    fprintf(out, "\tnot\n\tif-goto %s_%zu_%zu_0\n", className, i, gotoDepth);

    if (t->type == SYMBOL && t->data.symbol == ')') {
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        t = t->next;

        compStatements();

        if (t->type == SYMBOL && t->data.symbol == '}') {
          t = t->next;

          if (t->type == KEYWORD && t->data.keyword == ELSE) {
            fprintf(out, "\tgoto %s_%zu_%zu_1\n", className, i, gotoDepth);
            fprintf(out, "label %s_%zu_%zu_0\n", className, i, gotoDepth);
            t = t->next;

            if (t->type == SYMBOL && t->data.symbol == '{') {
              t = t->next;

              compStatements();

              if (t->type == SYMBOL && t->data.symbol == '}') {
                fprintf(out, "label %s_%zu_%zu_1\n", className, i, gotoDepth);
                t = t->next;

              } else
                errno = PARSING_ERROR;
            } else
              errno = PARSING_ERROR;

          } else {
            fprintf(out, "label %s_%zu_%zu_0\n", className, i, gotoDepth);
          }
        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  gotoDepth--;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid if statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compLet {{{2
void compLet() {
  // 'let' varName ('[' expression ']')? '=' expression ';'
  Symbol *symbol;
  bool isArray = false;
  if (t->type == IDENTIFIER) {
    symbol = st_get(sst, t->data.strVal);
    if (!symbol)
      symbol = st_get(cst, t->data.strVal);
    if (symbol) {
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '[') {
        isArray = true;
        t = t->next;

        compExpression();

        fprintf(out, "\tpush %s %zu\n\tadd\n", symbol_kind[symbol->kind],
                symbol->idx);

        if (t->type == SYMBOL && t->data.symbol == ']') {
          t = t->next;

        } else
          errno = PARSING_ERROR;
      }

      if (t->type == SYMBOL && t->data.symbol == '=') {
        t = t->next;

        compExpression();

        if (t->type == SYMBOL && t->data.symbol == ';') {

          if (isArray) {
            fprintf(out, "\tpop temp 0\n");
            fprintf(out, "\tpop pointer 1\n");
            fprintf(out, "\tpush temp 0\n");
            fprintf(out, "\tpop that 0\n");

          } else {
            switch (symbol->kind) {
            case LOCAL_:
              fprintf(out, "\tpop local %zu\n", symbol->idx);
              break;
            case ARGUMENT_:
              fprintf(out, "\tpop argument %zu\n", symbol->idx);
              break;
            case FIELD_:
              fprintf(out, "\tpop this %zu\n", symbol->idx);
              break;
            case STATIC_:
              fprintf(out, "\tpop static %zu\n", symbol->idx);
              break;
            default:
              break;
            }
          }
          t = t->next;

        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid let statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compSubroutineCall {{{2
void compSubroutineCall() {
  /* subroutineName '(' expressionList ')' | (className |
   * varName) '.' subroutineName '(' expressionList ')'  */
  Token *id1 = NULL, *id2 = NULL;
  Symbol *name = NULL;
  int nArgs = 0;
  if (t->type == IDENTIFIER) {
    name = st_get(sst, t->data.strVal);
    if (!name)
      name = st_get(cst, t->data.strVal);
    if (!name)
      id1 = t;

    t = t->next;

    if (t->type == SYMBOL && t->data.symbol == '.') {
      t = t->next;

      if (t->type == IDENTIFIER) {
        id2 = t;
        t = t->next;

      } else
        errno = PARSING_ERROR;
    } else
      fprintf(out, "\tpush pointer 0\n");

    if (t->type == SYMBOL && t->data.symbol == '(') {
      t = t->next;

      nArgs += compExpressionList();

      if (id2) {
        if (name) {
          fprintf(out, "\tpush %s %zu\n",
                  (name->kind == FIELD_) ? "this" : symbol_kind[name->kind],
                  name->idx);
          fprintf(out, "\tcall %s.%s %d\n", name->type, id2->data.strVal,
                  ++nArgs);

        } else
          fprintf(out, "\tcall %s.%s %d\n", id1->data.strVal, id2->data.strVal,
                  nArgs);

      } else
        fprintf(out, "\tcall %s.%s %d\n", className, id1->data.strVal, ++nArgs);

      if (t->type == SYMBOL && t->data.symbol == ')') {
        t = t->next;

      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid subroutine call\n",
            t->lineN, t->lineP);
    errno = 0;
  }
}
// compDo {{{2
void compDo() {
  // 'do' subroutineCall ';'

  compSubroutineCall();

  if (t->type == SYMBOL && t->data.symbol == ';') {
    fprintf(out, "\tpop temp 0\n");
    t = t->next;
  } else
    errno = PARSING_ERROR;

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid do statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compStatements {{{2
void compStatements() {
  /* (letStatement | ifStatement | whileStatement |
   * doStatement | returnStatement)* */
  for (;;) {
    if (t->type == KEYWORD && t->data.keyword == DO) {
      t = t->next;
      compDo();

    } else if (t->type == KEYWORD && t->data.keyword == LET) {
      t = t->next;
      compLet();

    } else if (t->type == KEYWORD && t->data.keyword == IF) {
      t = t->next;
      compIf();

    } else if (t->type == KEYWORD && t->data.keyword == WHILE) {
      t = t->next;
      compWhile();

    } else if (t->type == KEYWORD && t->data.keyword == RETURN) {
      t = t->next;
      compReturn();

    } else
      break;
  }

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compVarDec {{{2
int compVarDec() {
  // 'var' type varName (',' varName)* ';'
  int nVars = 0;
  Token *type;
  for (;;) {
    if (t->type == KEYWORD && t->data.keyword == VAR) {
      t = t->next;

      if ((t->type == KEYWORD &&
           (t->data.keyword == INT || t->data.keyword == CHAR ||
            t->data.keyword == BOOLEAN)) ||
          (t->type == IDENTIFIER)) {
        type = t;
        t = t->next;

        if (t->type == IDENTIFIER) {
          st_set(sst, t->data.strVal,
                 (type->type == KEYWORD) ? keywords[type->data.keyword]
                                         : type->data.strVal,
                 LOCAL_);
          nVars++;
          t = t->next;

          while (t->type == SYMBOL && t->data.symbol == ',') {
            t = t->next;

            if (t->type == IDENTIFIER) {
              st_set(sst, t->data.strVal,
                     (type->type == KEYWORD) ? keywords[type->data.keyword]
                                             : type->data.strVal,
                     LOCAL_);
              nVars++;
              t = t->next;

            } else {
              errno = PARSING_ERROR;
              break;
            }
          }

          if (t->type == SYMBOL && t->data.symbol == ';') {
            t = t->next;

          } else
            errno = PARSING_ERROR;
        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      break;
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid variable declaration\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  return nVars;
}
// compParameterList {{{2
void compParameterList() {
  // ((type varName) (',' type varName)*)?
  Token *type;
  if ((t->type == KEYWORD &&
       (t->data.keyword == INT || t->data.keyword == CHAR ||
        t->data.keyword == BOOLEAN)) ||
      (t->type == IDENTIFIER)) {
    type = t;
    t = t->next;

    if (t->type == IDENTIFIER) {
      st_set(sst, t->data.strVal,
             (type->type == KEYWORD) ? keywords[type->data.keyword]
                                     : type->data.strVal,
             ARGUMENT_);
      t = t->next;

      while (t->type == SYMBOL && t->data.symbol == ',') {
        t = t->next;

        if ((t->type == KEYWORD &&
             (t->data.keyword == INT || t->data.keyword == CHAR ||
              t->data.keyword == BOOLEAN)) ||
            (t->type == IDENTIFIER)) {
          type = t;
          t = t->next;

          if (t->type == IDENTIFIER) {
            st_set(sst, t->data.strVal,
                   (type->type == KEYWORD) ? keywords[type->data.keyword]
                                           : type->data.strVal,
                   ARGUMENT_);
            t = t->next;

          } else {
            errno = PARSING_ERROR;
            break;
          }

        } else {
          errno = PARSING_ERROR;
          break;
        }
      }

    } else
      errno = PARSING_ERROR;
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid parameters\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// compSubroutine {{{2
void compSubroutine() {
  /* 'constructor' | 'function' | 'method') ('void' | type) subroutineName
   * '(' parameterList ')' subroutineBody */
  for (;;) {
    Token *name;
    int nVars = 0;
    bool isConstructor = false;
    bool isMethod = false;
    sst = st_new();
    if (sst == NULL) {
      perror("Allocation error");
      return;
    }
    if (t->type == KEYWORD &&
        (t->data.keyword == CONSTRUCTOR || t->data.keyword == FUNCTION ||
         t->data.keyword == METHOD)) {
      if (t->data.keyword == METHOD) {
        isMethod = true;
      } else if (t->data.keyword == CONSTRUCTOR) {
        isConstructor = true;
      }
      t = t->next;

      if ((t->type == KEYWORD &&
           (t->data.keyword == VOID || t->data.keyword == INT ||
            t->data.keyword == CHAR || t->data.keyword == BOOLEAN)) ||
          (t->type == IDENTIFIER)) {
        if (isMethod && t->type == IDENTIFIER) {
          st_set(sst, "this", t->data.strVal, ARGUMENT_);
        }
        t = t->next;

        if (t->type == IDENTIFIER) {
          name = t;
          t = t->next;

          if (t->type == SYMBOL && t->data.symbol == '(') {
            t = t->next;

            compParameterList();

            if (t->type == SYMBOL && t->data.symbol == ')') {
              t = t->next;

              if (t->type == SYMBOL && t->data.symbol == '{') {
                t = t->next;

                nVars += compVarDec();

                fprintf(out, "function %s.%s %d\n", className,
                        name->data.strVal, nVars);
                if (isConstructor) {
                  fprintf(out, "\tpush constant %zu\n", cst->length);
                  fprintf(out, "\tcall Memory.alloc 1\n");
                  fprintf(out, "\tpop pointer 0\n");
                } else if (isMethod) {
                  fprintf(out, "\tpush argument 0\n");
                  fprintf(out, "\tpop pointer 0\n");
                }

                compStatements();

                if (t->type == SYMBOL && t->data.symbol == '}') {
                  t = t->next;

                } else
                  errno = PARSING_ERROR;
              } else
                errno = PARSING_ERROR;
            } else
              errno = PARSING_ERROR;
          } else
            errno = PARSING_ERROR;
        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
      sst = st_del(sst);
    } else {
      sst = st_del(sst);
      break;
    }
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid subroutine declaration\n",
            t->lineN, t->lineP);
    fprintf(stderr, "%c\n", t->data.symbol);
    errno = 0;
  }
}
// compClassVarDec{{{2
void compClassVarDec() {
  // ('static' | 'field') type varName (',' varName)* ';'
  Token *kind, *type;
  for (;;) {
    if (t->type == KEYWORD &&
        (t->data.keyword == STATIC || t->data.keyword == FIELD)) {
      kind = t;
      t = t->next;

      if ((t->type == KEYWORD &&
           (t->data.keyword == INT || t->data.keyword == CHAR ||
            t->data.keyword == BOOLEAN)) ||
          (t->type == IDENTIFIER)) {
        type = t;
        t = t->next;

        if (t->type == IDENTIFIER) {
          st_set(cst, t->data.strVal,
                 (type->type == KEYWORD) ? keywords[type->data.keyword]
                                         : type->data.strVal,
                 (kind->data.keyword == STATIC) ? STATIC_ : FIELD_);
          t = t->next;

          while (t->type == SYMBOL && t->data.symbol == ',') {
            t = t->next;

            if (t->type == IDENTIFIER) {
              st_set(cst, t->data.strVal,
                     (type->type == KEYWORD) ? keywords[type->data.keyword]
                                             : type->data.strVal,
                     (kind->data.keyword == STATIC) ? STATIC_ : FIELD_);
              t = t->next;
            } else {
              errno = PARSING_ERROR;
              break;
            }
          }

          if (t->type == SYMBOL && t->data.symbol == ';') {
            t = t->next;

          } else
            errno = PARSING_ERROR;
        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      break;
  }
  if (errno) {
    fprintf(stderr,
            "[%zu:%zu] Syntax error: invalid class variable declaration\n",
            t->lineN, t->lineP);
    errno = 0;
  }
}
// compClass {{{2
void compClass() {
  //'class' className '{' classVarDec* subroutineDec* '}'
  cst = st_new();
  if (cst == NULL) {
    perror("Allocation error");
    return;
  }
  if (t->type == KEYWORD && t->data.keyword == CLASS) {
    t = t->next;

    if (t->type == IDENTIFIER) {
      strcpy((char *)className, t->data.strVal);
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        t = t->next;

        compClassVarDec();
        compSubroutine();

        if (t->data.symbol != '}')
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid class\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  cst = st_del(cst);
}
// handle_file {{{1
void handle_file(FILE *in) {
  TokenList *tl = tokenize_file(in);
  // token_list_dump(tl);
  t = tl->head;
  compClass();
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
        out = fopen(path, "w");
        if (out) {
          handle_file(file);
          fclose(out);
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
                out = fopen(file_path, "w");
                if (out) {
                  handle_file(file);
                  fclose(out);
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
