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
  return tl;
}
// compilation engine {{{1
// prototypes {{{2
Token *compExpressionList(Token *t, FILE *out);
Token *compTerm(Token *t, FILE *out);
Token *compExpression(Token *t, FILE *out);
Token *compReturn(Token *t, FILE *out);
Token *compWhile(Token *t, FILE *out);
Token *compIf(Token *t, FILE *out);
Token *compLet(Token *t, FILE *out);
Token *compSubroutineCall(Token *t, FILE *out);
Token *compDo(Token *t, FILE *out);
Token *compStatements(Token *t, FILE *out);
Token *compVarDec(Token *t, FILE *out);
Token *compParameterList(Token *t, FILE *out);
Token *compSubroutine(Token *t, FILE *out);
Token *compClassVarDec(Token *t, FILE *out);
void compClass(Token *t, FILE *out);
// compExpressionList {{{2
Token *compExpressionList(Token *t, FILE *out) {
  // (expression (',' expression)* )?
  fprintf(out, "<expressionList>\n");

  t = compExpression(t, out);

  while (t->type == SYMBOL && t->data.symbol == ',') {
    fprintf(out, "<symbol>,</symbol>\n");
    t = t->next;

    t = compExpression(t, out);
  }

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid expression list\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  fprintf(out, "</expressionList>\n");
  return t;
}
// compTerm {{{2
Token *compTerm(Token *t, FILE *out) {
  /*   integerConstant | stringConstant | keywordConstant |
   * varName | varName '[' expression ']' | subroutineCall |
   * '(' expression ')' | unaryOp term */
  Token *prev;
  if (t->type == INT_CONST) {
    fprintf(out, "<term>\n<integerConstant>%d</integerConstant>\n</term>\n",
            t->data.intVal);
    t = t->next;

  } else if (t->type == STR_CONST) {
    fprintf(out, "<term>\n<stringConstant>%s</stringConstant>\n</term>\n",
            t->data.strVal);
    t = t->next;

  } else if (t->type == KEYWORD &&
             (t->data.keyword == TRUE || t->data.keyword == FALSE ||
              t->data.keyword == NUL || t->data.keyword == THIS)) {
    fprintf(out, "<term>\n<keyword>%s</keyword>\n</term>\n",
            keywords[t->data.keyword]);
    t = t->next;

  } else if (t->type == IDENTIFIER) {
    fprintf(out, "<term>\n");
    prev = t;
    t = t->next;

    if (t->type == SYMBOL && t->data.symbol == '[') {
      fprintf(out, "<identifier>%s</identifier>\n<symbol>[</symbol>\n",
              prev->data.strVal);
      t = t->next;

      t = compExpression(t, out);

      if (t->type == SYMBOL && t->data.symbol == ']') {
        fprintf(out, "<symbol>]</symbol>\n");
        t = t->next;

      } else
        errno = PARSING_ERROR;

    } else if (t->type == SYMBOL &&
               (t->data.symbol == '(' || t->data.symbol == '.')) {
      t = compSubroutineCall(prev, out);

    } else {
      fprintf(out, "<identifier>%s</identifier>\n", prev->data.strVal);
    }
    fprintf(out, "</term>\n");

  } else if (t->type == SYMBOL && t->data.symbol == '(') {
    fprintf(out, "<term>\n<symbol>(</symbol>\n");
    t = t->next;

    t = compExpression(t, out);

    if (t->type == SYMBOL && t->data.symbol == ')') {
      fprintf(out, "<symbol>)</symbol>\n");
      t = t->next;

    } else
      errno = PARSING_ERROR;
    fprintf(out, "</term>\n");

  } else if (t->type == SYMBOL &&
             (t->data.symbol == '-' || t->data.symbol == '~')) {
    fprintf(out, "<term>\n<symbol>%c</symbol>\n", t->data.symbol);
    t = t->next;

    t = compTerm(t, out);

    fprintf(out, "</term>\n");
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid term\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  return t;
}
// compExpression {{{2
Token *compExpression(Token *t, FILE *out) {
  // term (op term)*
  if ((t->type == INT_CONST) || (t->type == STR_CONST) ||
      ((t->type == KEYWORD &&
        (t->data.keyword == TRUE || t->data.keyword == FALSE ||
         t->data.keyword == NUL || t->data.keyword == THIS))) ||
      (t->type == IDENTIFIER) || (t->type == SYMBOL && t->data.symbol == '(') ||
      (t->type == SYMBOL && (t->data.symbol == '-' || t->data.symbol == '~'))) {
    fprintf(out, "<expression>\n");

    t = compTerm(t, out);

    while (t->type == SYMBOL &&
           (t->data.symbol == '+' || t->data.symbol == '-' ||
            t->data.symbol == '*' || t->data.symbol == '/' ||
            t->data.symbol == '&' || t->data.symbol == '|' ||
            t->data.symbol == '<' || t->data.symbol == '>' ||
            t->data.symbol == '=')) {

      switch (t->data.symbol) {
      case '<':
        fprintf(out, "<symbol>&lt;");
        break;
      case '>':
        fprintf(out, "<symbol>&gt;");
        break;
      case '&':
        fprintf(out, "<symbol>&amp;");
        break;
      default:
        fprintf(out, "<symbol>%c", t->data.symbol);
      }
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '=') {
        fprintf(out, "=</symbol>\n");
        t = t->next;
      } else
        fprintf(out, "</symbol>\n");

      t = compTerm(t, out);
    }
    fprintf(out, "</expression>\n");
  }

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid expression\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  return t;
}
// compReturn {{{2
Token *compReturn(Token *t, FILE *out) {
  // 'return' expression? ';'
  fprintf(out, "<returnStatement>\n<keyword>return</keyword>\n");

  t = compExpression(t, out);

  if (t->type == SYMBOL && t->data.symbol == ';') {
    fprintf(out, "<symbol>;</symbol>\n");
    t = t->next;

  } else
    errno = PARSING_ERROR;

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid return statement\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  fprintf(out, "</returnStatement>\n");
  return t;
}
// compWhile {{{2
Token *compWhile(Token *t, FILE *out) {
  // 'while' '(' expression ')' '{' statements '}'
  fprintf(out, "<whileStatement>\n<keyword>while</keyword>\n");
  if (t->type == SYMBOL && t->data.symbol == '(') {
    fprintf(out, "<symbol>(</symbol>\n");
    t = t->next;

    t = compExpression(t, out);

    if (t->type == SYMBOL && t->data.symbol == ')') {
      fprintf(out, "<symbol>)</symbol>\n");
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        fprintf(out, "<symbol>{</symbol>\n");
        t = t->next;

        t = compStatements(t, out);

        if (t->type == SYMBOL && t->data.symbol == '}') {
          fprintf(out, "<symbol>}</symbol>\n");
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
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid while statement\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  fprintf(out, "</whileStatement>\n");
  return t;
}
// compIf {{{2
Token *compIf(Token *t, FILE *out) {
  /*   'if' '(' expression ')' '{' statements '}'
   * ('else' '{' statements '}')? */
  fprintf(out, "<ifStatement>\n<keyword>if</keyword>\n");
  if (t->type == SYMBOL && t->data.symbol == '(') {
    fprintf(out, "<symbol>(</symbol>\n");
    t = t->next;

    t = compExpression(t, out);

    if (t->type == SYMBOL && t->data.symbol == ')') {
      fprintf(out, "<symbol>)</symbol>\n");
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        fprintf(out, "<symbol>{</symbol>\n");
        t = t->next;

        t = compStatements(t, out);

        if (t->type == SYMBOL && t->data.symbol == '}') {
          fprintf(out, "<symbol>}</symbol>\n");
          t = t->next;

          if (t->type == KEYWORD && t->data.keyword == ELSE) {
            fprintf(out, "<keyword>else</keyword>\n");
            t = t->next;

            if (t->type == SYMBOL && t->data.symbol == '{') {
              fprintf(out, "<symbol>{</symbol>\n");
              t = t->next;

              t = compStatements(t, out);

              if (t->type == SYMBOL && t->data.symbol == '}') {
                fprintf(out, "<symbol>}</symbol>\n");
                t = t->next;

              } else
                errno = PARSING_ERROR;
            } else
              errno = PARSING_ERROR;
          }
        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
  } else
    errno = PARSING_ERROR;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid if statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  fprintf(out, "</ifStatement>\n");
  return t;
}
// compLet {{{2
Token *compLet(Token *t, FILE *out) {
  // 'let' varName ('[' expression ']')? '=' expression ';'
  fprintf(out, "<letStatement>\n<keyword>let</keyword>\n");
  if (t->type == IDENTIFIER) {
    fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
    t = t->next;

    if (t->type == SYMBOL && t->data.symbol == '[') {
      fprintf(out, "<symbol>[</symbol>\n");
      t = t->next;

      t = compExpression(t, out);

      if (t->type == SYMBOL && t->data.symbol == ']') {
        fprintf(out, "<symbol>]</symbol>\n");
        t = t->next;

      } else
        errno = PARSING_ERROR;
    }

    if (t->type == SYMBOL && t->data.symbol == '=') {
      fprintf(out, "<symbol>=</symbol>\n");
      t = t->next;

      t = compExpression(t, out);

      if (t->type == SYMBOL && t->data.symbol == ';') {
        fprintf(out, "<symbol>;</symbol>\n");
        t = t->next;

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
  fprintf(out, "</letStatement>\n");
  return t;
}
// compSubroutineCall {{{2
Token *compSubroutineCall(Token *t, FILE *out) {
  /* subroutineName '(' expressionList ')' | (className |
   * varName) '.' subroutineName '(' expressionList ')'  */
  if (t->type == IDENTIFIER) {
    fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
    t = t->next;

    if (t->type == SYMBOL && t->data.symbol == '.') {
      fprintf(out, "<symbol>.</symbol>\n");
      t = t->next;

      if (t->type == IDENTIFIER) {
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
        t = t->next;

      } else
        errno = PARSING_ERROR;
    }

    if (t->type == SYMBOL && t->data.symbol == '(') {
      fprintf(out, "<symbol>(</symbol>\n");
      t = t->next;

      t = compExpressionList(t, out);

      if (t->type == SYMBOL && t->data.symbol == ')') {
        fprintf(out, "<symbol>)</symbol>\n");
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
  return t;
}
// compDo {{{2
Token *compDo(Token *t, FILE *out) {
  // 'do' subroutineCall ';'
  fprintf(out, "<doStatement>\n<keyword>do</keyword>\n");

  t = compSubroutineCall(t, out);

  if (t->type == SYMBOL && t->data.symbol == ';') {
    fprintf(out, "<symbol>;</symbol>\n");
    t = t->next;
  } else
    errno = PARSING_ERROR;

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid do statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  fprintf(out, "</doStatement>\n");
  return t;
}
// compStatements {{{2
Token *compStatements(Token *t, FILE *out) {
  /* (letStatement | ifStatement | whileStatement |
   * doStatement | returnStatement)* */
  Token *n;
  fprintf(out, "<statements>\n");
  while (true) {
    if (t->type == KEYWORD && t->data.keyword == DO) {
      t = t->next;
      n = compDo(t, out);
      if (n == t)
        break;
      t = n;

    } else if (t->type == KEYWORD && t->data.keyword == LET) {
      t = t->next;
      n = compLet(t, out);
      if (n == t)
        break;
      t = n;

    } else if (t->type == KEYWORD && t->data.keyword == IF) {
      t = t->next;
      n = compIf(t, out);
      if (n == t)
        break;
      t = n;

    } else if (t->type == KEYWORD && t->data.keyword == WHILE) {
      t = t->next;
      n = compWhile(t, out);
      if (n == t)
        break;
      t = n;

    } else if (t->type == KEYWORD && t->data.keyword == RETURN) {
      t = t->next;
      n = compReturn(t, out);
      if (n == t)
        break;
      t = n;
    } else
      break;
  }
  fprintf(out, "</statements>\n");

  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid statement\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  return t;
}
// compVarDec {{{2
Token *compVarDec(Token *t, FILE *out) {
  // 'var' type varName (',' varName)* ';'
  if (t->type == KEYWORD && t->data.keyword == VAR) {
    fprintf(out, "<varDec>\n<keyword>var</keyword>\n");
    t = t->next;

    if ((t->type == KEYWORD &&
         (t->data.keyword == INT || t->data.keyword == CHAR ||
          t->data.keyword == BOOLEAN)) ||
        (t->type == IDENTIFIER)) {
      if (t->type == KEYWORD)
        fprintf(out, "<keyword>%s</keyword>\n", keywords[t->data.keyword]);
      else
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
      t = t->next;

      if (t->type == IDENTIFIER) {
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
        t = t->next;

        while (true) {
          if (t->type != SYMBOL || t->data.symbol != ',')
            break;
          fprintf(out, "<symbol>,</symbol>\n");
          t = t->next;

          if (t->type == IDENTIFIER) {
            fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
            t = t->next;
          } else {
            errno = PARSING_ERROR;
            break;
          }
        }

        if (t->type == SYMBOL && t->data.symbol == ';') {
          fprintf(out, "<symbol>;</symbol>\n");
          t = t->next;

        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
    fprintf(out, "</varDec>\n");
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid variable declaration\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  return t;
}
// compParameterList {{{2
Token *compParameterList(Token *t, FILE *out) {
  // ((type varName) (',' type varName)*)?
  fprintf(out, "<parameterList>\n");
  if ((t->type == KEYWORD &&
       (t->data.keyword == INT || t->data.keyword == CHAR ||
        t->data.keyword == BOOLEAN)) ||
      (t->type == IDENTIFIER)) {
    if (t->type == KEYWORD)
      fprintf(out, "<keyword>%s</keyword>\n", keywords[t->data.keyword]);
    else
      fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
    t = t->next;

    if (t->type == IDENTIFIER) {
      fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
      t = t->next;

      while (t->type == SYMBOL && t->data.symbol == ',') {
        fprintf(out, "<symbol>,</symbol>\n");
        t = t->next;

        if ((t->type == KEYWORD &&
             (t->data.keyword == INT || t->data.keyword == CHAR ||
              t->data.keyword == BOOLEAN)) ||
            (t->type == IDENTIFIER)) {
          if (t->type == KEYWORD)
            fprintf(out, "<keyword>%s</keyword>\n", keywords[t->data.keyword]);
          else
            fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
          t = t->next;

          if (t->type == IDENTIFIER) {
            fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
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
  fprintf(out, "</parameterList>\n");
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid parameters\n", t->lineN,
            t->lineP);
    errno = 0;
  }
  return t;
}
// compSubroutine {{{2
Token *compSubroutine(Token *t, FILE *out) {
  /* 'constructor' | 'function' | 'method') ('void' | type) subroutineName
   * '(' parameterList ')' subroutineBody */
  Token *n;
  if (t->type == KEYWORD &&
      (t->data.keyword == CONSTRUCTOR || t->data.keyword == FUNCTION ||
       t->data.keyword == METHOD)) {
    fprintf(out, "<subroutineDec>\n<keyword>%s</keyword>\n",
            keywords[t->data.keyword]);
    t = t->next;

    if ((t->type == KEYWORD &&
         (t->data.keyword == VOID || t->data.keyword == INT ||
          t->data.keyword == CHAR || t->data.keyword == BOOLEAN)) ||
        (t->type == IDENTIFIER)) {
      if (t->type == KEYWORD)
        fprintf(out, "<keyword>%s</keyword>\n", keywords[t->data.keyword]);
      else
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
      t = t->next;

      if (t->type == IDENTIFIER) {
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
        t = t->next;

        if (t->type == SYMBOL && t->data.symbol == '(') {
          fprintf(out, "<symbol>(</symbol>\n");
          t = t->next;

          t = compParameterList(t, out);

          if (t->type == SYMBOL && t->data.symbol == ')') {
            fprintf(out, "<symbol>)</symbol>\n");
            t = t->next;

            if (t->type == SYMBOL && t->data.symbol == '{') {
              fprintf(out, "<subroutineBody>\n<symbol>{</symbol>\n");
              t = t->next;

              while (true) {
                n = compVarDec(t, out);
                if (n == t)
                  break;
                t = n;
              }

              t = compStatements(t, out);

              if (t->type == SYMBOL && t->data.symbol == '}') {
                fprintf(out, "<symbol>}</symbol>\n</subroutineBody>\n");
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
    fprintf(out, "</subroutineDec>\n");
  }
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid subroutine declaration\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  return t;
}
// compClassVarDec{{{2
Token *compClassVarDec(Token *t, FILE *out) {
  // ('static' | 'field') type varName (',' varName)* ';'
  if (t->type == KEYWORD &&
      (t->data.keyword == STATIC || t->data.keyword == FIELD)) {
    fprintf(out, "<classVarDec>\n<keyword>%s</keyword>\n",
            keywords[t->data.keyword]);
    t = t->next;

    if ((t->type == KEYWORD &&
         (t->data.keyword == INT || t->data.keyword == CHAR ||
          t->data.keyword == BOOLEAN)) ||
        (t->type == IDENTIFIER)) {
      if (t->type == KEYWORD)
        fprintf(out, "<keyword>%s</keyword>\n", keywords[t->data.keyword]);
      else
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
      t = t->next;

      if (t->type == IDENTIFIER) {
        fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
        t = t->next;

        while (true) {
          if (t->type != SYMBOL || t->data.symbol != ',')
            break;
          fprintf(out, "<symbol>,</symbol>\n");
          t = t->next;

          if (t->type == IDENTIFIER) {
            fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
            t = t->next;
          } else {
            errno = PARSING_ERROR;
            break;
          }
        }

        if (t->type == SYMBOL && t->data.symbol == ';') {
          fprintf(out, "<symbol>;</symbol>\n");
          t = t->next;

        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
    fprintf(out, "</classVarDec>\n");
  }
  if (errno) {
    fprintf(stderr,
            "[%zu:%zu] Syntax error: invalid class variable declaration\n",
            t->lineN, t->lineP);
    errno = 0;
  }
  return t;
}
// compClass {{{2
void compClass(Token *t, FILE *out) {
  //'class' className '{' classVarDec* subroutineDec* '}'
  Token *n;
  if (t->type == KEYWORD && t->data.keyword == CLASS) {
    fprintf(out, "<class>\n<keyword>class</keyword>\n");
    t = t->next;

    if (t->type == IDENTIFIER) {
      fprintf(out, "<identifier>%s</identifier>\n", t->data.strVal);
      t = t->next;

      if (t->type == SYMBOL && t->data.symbol == '{') {
        fprintf(out, "<symbol>{</symbol>\n");
        t = t->next;

        while (true) {
          n = compClassVarDec(t, out);
          if (n == t)
            break;
          t = n;
        }

        while (true) {
          n = compSubroutine(t, out);
          if (n == t)
            break;
          t = n;
        }

        if (t->type == SYMBOL && t->data.symbol == '}') {
          fprintf(out, "<symbol>}</symbol>\n");

        } else
          errno = PARSING_ERROR;
      } else
        errno = PARSING_ERROR;
    } else
      errno = PARSING_ERROR;
    fprintf(out, "</class>\n");
  } else
    errno = PARSING_ERROR;
  if (errno) {
    fprintf(stderr, "[%zu:%zu] Syntax error: invalid class\n", t->lineN,
            t->lineP);
    errno = 0;
  }
}
// handle_file {{{1
void handle_file(FILE *in, FILE *out) {
  TokenList *tl = tokenize_file(in);
  // token_list_dump(tl);
  compClass(tl->head, out);
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
        strcpy(dot, ".xml");
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
                strcpy(dot, ".xml");
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
