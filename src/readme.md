## Introduction

This is a walktrough of my implementation of Jack Compiler for the Nand2Tetris course in C programming language. It is a 3-stage compiler. High level Jack code is first translated into "bytecode" for Jack Virtual Machine. Than VM code is translated into assembly code, which is in turn translated into Hack machine code. We will start with assembler first, as it is the way it progresses in the course and is a more intuitive approach anyway. To keep this article as terse as possible I will omit some of the code that is irrelevant to this specific topic (like handling input/output) or is repetitive. The complete code is available on my [github](https://github.com/gunnerson/n2t/tree/master/src).

## Assembler

### Hack computer overview

The Hack computer is a von Neumann platform. It is a 16-bit machine, consisting of a CPU, two separate memory modules serving as instruction memory and data memory, and two memory-mapped I/O devices: a screen and a keyboard. Both memories are 16-bit wide and have a 15-bit address space. The CPU can only execute programs that reside in the instruction memory. The instruction memory is a read-only device, and programs are loaded into it using some exogenous means.

The Hack programmer is aware of two 16-bit registers, which can be manipulated explicitly by arithmetic and logical instructions. While D is used solely to store data values, A doubles as both a data register and an address register. C-instructions (computation instructions) has “1” as the most significant bit. The remaining 15 bits are bit-field encoded to define the operands, computation performed, and storage location for the specified computation result. This instruction may also specify a program branch based on the most recent computation result. The instruction syntax is defined by three fields, referred to as “comp”, “dest”, and “jump”.

| Line Nbr | Hack Assembly Language Program | Operating Notes | Instruction Type | ROM Addr | Hack Machine Code|
|----------|--------------------------------|-----------------|------------------|:--------:|:----------------:|
| 01 | // Add consecutive integers 1 thru 100 | Comment that describes program action | Full-line comment | - - - - | No code generated |
| 02 | // sum = 1 + 2 + 3 + ... + 99 + 100 | Comments are ignored by assembler | Full-line comment | - - - - | No code generated |
| 03 |  | Blank source lines are ignored by assembler | Blank line | - - - - | No code generated |
| 04 | @cnt // loop counter declaration | Variable symbol "cnt" bound to 16 | A-instruction | 00 | 0000000000010000 |
| 05 | M=1 // initialize loop counter to 1 | RAM[16] ← 1 | C-instruction | 01 | 1110111111001000 |
| 06 | @sum // sum accumulator declaration | Variable symbol "sum" bound to 17 | A-instruction | 02 | 0000000000010001 |
| 07 | M=0 // initialize sum to 0 | RAM[17] ← 0 | C-instruction | 03 | 1110101010001000 |
| 08 | (LOOP) // start of while loop | Label symbol bound to ROM address 04 | Label declaration | - - - - | No code generated |
| 09 | @cnt // reference addr of cnt | M ← 16 | A-instruction | 04 | 0000000000010000 |
| 10 | D=M // move current cnt value to D | D ← RAM[16] | C-instruction | 05 | 1111110000010000 |
| 11 | @100 // load loop limit into A | A ← 100 | A-instruction | 06 | 0000000001100100 |
| 12 | D=D-A // perform loop test computation | D ← D - A | C-instruction | 07 | 1110010011010000 |
| 13 | @END // load target destination for branch | M ← 18 | A-instruction | 08 | 0000000000010010 |
| 14 | D;JGT //exit loop if D > 0 | Conditional branch | C-instruction | 09 | 1110001100000001 |
| 15 | @cnt // reference addr of cnt | M ← 16 | A-instruction | 10 | 0000000000010000 |
| 16 | D=M // move current cnt value to D | D ← RAM[16] | C-instruction | 11 | 1111110000010000 |
| 17 | @sum // reference address of sum | M ← 17 | A-instruction | 12 | 0000000000010001 |
| 18 | M=D+M // add cnt to sum | M ← D + RAM[17] | C-instruction | 13 | 1111000010001000 |
| 19 | @cnt // reference addr of cnt | M ← 16 | A-instruction | 14 | 0000000000010000 |
| 20 | M=M+1 // increment counter | RAM[16] ← RAM[16] + 1 | C-instruction | 15 | 1111110111001000 |
| 21 | @LOOP // load target destination for branch | M ← 4 | A-instruction | 16 | 0000000000000100 |
| 22 | 0;JMP // jump to LOOP entry | Unconditional branch | C-instruction | 17 | 1110101010000111 |
| 23 | (END) // start of terminating loop | Label symbol bound to ROM address 18 | Label declaration | - - - - | No code generated |
| 24 | @END // load target destination for branch | M ← 18 | A-instruction | 18 | 0000000000010010 |
| 25 | 0;JMP // jump to END entry | Unconditional branch | C-instruction | 19 | 1110101010000111 |

### Key-value store

The programmer can use symbolic variable names or mark various locations in the program with labels. In order to translate them into valid memory addresses we will make the assembler run two passes. On the first pass it will identify all GOTO labels and store them in a hash table. On the second pass it will parse all instructions while using following hash table to store and lookup all variable names and labels.

~~~c
#define INITIAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct {
  const char *key;
  unsigned value;
} Symbol;
typedef struct {
  Symbol *entries;
  size_t capacity;
  size_t length;
} SymbolTable;

SymbolTable *st_new(void) {
  SymbolTable *self = malloc(sizeof(SymbolTable));
  if (self == NULL) {
    return NULL;
  }
  self->length = 0;
  self->capacity = INITIAL_CAPACITY;
  self->entries = calloc(self->capacity, sizeof(Symbol));
  if (self->entries == NULL) {
    free(self);
    return NULL;
  }
  return self;
}

void st_del(SymbolTable *self) {
  for (size_t i = 0; i < self->capacity; i++) {
    free((void *)self->entries[i].key);
  }
  free(self->entries);
  free(self);
}

static uint64_t st_hash(const char *key) {
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

unsigned st_get(SymbolTable *self, const char *key) {
  uint64_t hash = st_hash(key);
  size_t index = (size_t)(hash & (uint64_t)(self->capacity - 1));
  while (self->entries[index].key != NULL) {
    if (strcmp(key, self->entries[index].key) == 0) {
      return self->entries[index].value;
    }
    index++;
    if (index >= self->capacity) {
      index = 0;
    }
  }
  return 0;
}

static const char *st_set_entry(Symbol *entries, size_t capacity,
                                const char *key, unsigned value,
                                size_t *length_ptr) {
  uint64_t hash = st_hash(key);
  size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
  while (entries[index].key != NULL) {
    if (strcmp(key, entries[index].key) == 0) {
      entries[index].value = value;
      return entries[index].key;
    }
    index++;
    if (index >= capacity) {
      index = 0;
    }
  }
  if (length_ptr != NULL) {
    key = strdup(key);
    if (key == NULL) {
      return NULL;
    }
    (*length_ptr)++;
  }
  entries[index].key = (char *)key;
  entries[index].value = value;
  return key;
}

static bool st_expand(SymbolTable *self) {
  size_t new_capacity = self->capacity * 2;
  if (new_capacity < self->capacity) {
    return false;
  }
  Symbol *new_entries = calloc(new_capacity, sizeof(Symbol));
  if (new_entries == NULL) {
    return false;
  }
  for (size_t i = 0; i < self->capacity; i++) {
    Symbol entry = self->entries[i];
    if (entry.key != NULL) {
      st_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
    }
  }
  free(self->entries);
  self->entries = new_entries;
  self->capacity = new_capacity;
  return true;
}

const char *st_set(SymbolTable *self, const char *key, unsigned value) {
  if (self->length >= self->capacity / 2) {
    if (!st_expand(self)) {
      return NULL;
    }
  }
  return st_set_entry(self->entries, self->capacity, key, value, &self->length);
}
~~~

### First pass

Before the first pass we initialize the symbole table and fill it up with standart instructions.

~~~c
#define SCREEN_ADDRESS 16384U
#define KEYBOARD_ADDRESS 24576U

SymbolTable *symbols = st_new();
if (symbols == NULL) {
return EXIT_FAILURE;
}
st_set(symbols, "SP", 0);
st_set(symbols, "LCL", 1);
st_set(symbols, "ARG", 2);
st_set(symbols, "THIS", 3);
st_set(symbols, "THAT", 4);
st_set(symbols, "R0", 0);
st_set(symbols, "R1", 1);
st_set(symbols, "R2", 2);
st_set(symbols, "R3", 3);
st_set(symbols, "R4", 4);
st_set(symbols, "R5", 5);
st_set(symbols, "R6", 6);
st_set(symbols, "R7", 7);
st_set(symbols, "R8", 8);
st_set(symbols, "R9", 9);
st_set(symbols, "R10", 10);
st_set(symbols, "R11", 11);
st_set(symbols, "R12", 12);
st_set(symbols, "R13", 13);
st_set(symbols, "R14", 14);
st_set(symbols, "R15", 15);
st_set(symbols, "SCREEN", SCREEN_ADDRESS);
st_set(symbols, "KBD", KEYBOARD_ADDRESS);

st_set(symbols, "0", 42);
st_set(symbols, "1", 63);
st_set(symbols, "-1", 58);
st_set(symbols, "D", 12);
st_set(symbols, "A", 48);
st_set(symbols, "!D", 13);
st_set(symbols, "!A", 49);
st_set(symbols, "-D", 15);
st_set(symbols, "-A", 51);
st_set(symbols, "D+1", 31);
st_set(symbols, "A+1", 55);
st_set(symbols, "D-1", 14);
st_set(symbols, "A-1", 50);
st_set(symbols, "D+A", 2);
st_set(symbols, "D-A", 19);
st_set(symbols, "A-D", 7);
st_set(symbols, "D&A", 0);
st_set(symbols, "D|A", 21);
st_set(symbols, "M", 112);
st_set(symbols, "!M", 113);
st_set(symbols, "-M", 115);
st_set(symbols, "M+1", 119);
st_set(symbols, "M-1", 114);
st_set(symbols, "D+M", 66);
st_set(symbols, "D-M", 83);
st_set(symbols, "M-D", 71);
st_set(symbols, "D&M", 64);
st_set(symbols, "D|M", 85);

st_set(symbols, "@M", 1);
st_set(symbols, "@D", 2);
st_set(symbols, "MD", 3);
st_set(symbols, "@A", 4);
st_set(symbols, "AM", 5);
st_set(symbols, "AD", 6);
st_set(symbols, "AMD", 7);

st_set(symbols, "JGT", 1);
st_set(symbols, "JEQ", 2);
st_set(symbols, "JGE", 3);
st_set(symbols, "JLT", 4);
st_set(symbols, "JNE", 5);
st_set(symbols, "JLE", 6);
st_set(symbols, "JMP", 7);
~~~

Now we can run first pass and add all GOTO lables to the table.

~~~c

#define MAX_LINE_LENGTH 128
#define MAX_ADDRESS 32767U

char line[MAX_LINE_LENGTH] = {0};
unsigned instrNumber = 0;
for (size_t lineNumber = 1; fgets(line, MAX_LINE_LENGTH, file);
   lineNumber++) {
  size_t j = 0;
  char symbol[MAX_LINE_LENGTH] = {0};
  bool isLabel = false;
  bool isInstr = false;

  for (size_t i = 0; i < strlen(line); i++) {
    char c = line[i];

    if (isLabel) {
      if (c == ')') {
        break;
      } else if (c == '_' || c == '.' || c == '$' || c == ':' ||
                 (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
                 (c >= 'A' && c <= 'Z')) {
        symbol[j++] = c;
        continue;
      }
      EXIT_ERROR("Invalid label");
    }

    if (c == '\r' || c == '\n' || c == '/') {
      break;
    }
    if (c == ' ' || c == '\t')
      continue;
    if (c == '(') {
      isLabel = true;
      continue;
    }
    if (c == '@' || c == '0' || c == '1' || c == '-' || c == 'D' ||
        c == 'A' || c == 'M' || c == '!') {
      isInstr = true;
      break;
    }
    EXIT_ERROR("Invalid instruction");
  }

  if (*symbol) {
    if (instrNumber > MAX_ADDRESS) {
      EXIT_ERROR("Instruction address limit reached");
    }
    st_set(symbols, symbol, instrNumber);
  } else if (isInstr) {
    instrNumber++;
  }
}
~~~


Finally on the last pass we line-by-line translate every assembly instruction to machine code.

~~~c
#define MAX_TOKEN_LENGTH 8

unsigned nextAddress = START_SYMBOL_ADDRESS;
for (size_t lineNumber = 1; fgets(line, MAX_LINE_LENGTH, file);
   lineNumber++) {
char aInstr[MAX_LINE_LENGTH] = {0};
char token[MAX_LINE_LENGTH] = {0};
char opcode[MAX_TOKEN_LENGTH] = {0};
char dest[MAX_TOKEN_LENGTH] = {0};
char comp[MAX_TOKEN_LENGTH] = {0};
char jmp[MAX_TOKEN_LENGTH] = {0};
size_t j = 0;
unsigned addr;
unsigned comp_d = 0;
unsigned dest_d = 0;
unsigned jmp_d = 0;
bool hasJump = false;

for (size_t i = 0; i < strlen(line); i++) {
  char c = line[i];

  if (c == '/' || c == '\r' || c == '\n' || c == '(')
    break;
  if (!*opcode) {
    if (c == ' ' || c == '\t')
      continue;
    if (c == '@') {
      strcpy(opcode, "0");
    } else {
      strcpy(opcode, "111");
      token[j++] = c;
    }

  } else {
    if (isspace(c))
      break;
    if (opcode[0] == '0') {
      aInstr[j++] = c;

    } else {
      if (c == '=') {
        if (!strcmp(token, "M") || !strcmp(token, "D") ||
            !strcmp(token, "A")) {
          snprintf(dest, MAX_TOKEN_LENGTH, "@%s", token);
        } else
          strcpy(dest, token);
        j = 0;
      } else if (c == ';') {
        strcpy(comp, token);
        j = 0;
        hasJump = true;
      } else
        token[j++] = c;
    }
  }
}

if (opcode[0] == '0') {
  if (isdigit(aInstr[0])) {
    addr = atoi(aInstr);
    if (addr > MAX_ADDRESS) {
      EXIT_ERROR("Address out of bounds");
    }
  } else {
    if (!strcmp(aInstr, "SP") || !strcmp(aInstr, "R0")) {
      addr = 0;
    } else {
      addr = st_get(symbols, aInstr);
      if (!addr) {
        if (nextAddress >= SCREEN_ADDRESS) {
          EXIT_ERROR("Instruction address limit reached");
        }
        addr = nextAddress++;
        st_set(symbols, aInstr, addr);
      }
    }
  }
  fprintf(output, "%s%015b\n", opcode, addr);

} else if (opcode[0] == '1') {
  (hasJump) ? strcpy(jmp, token) : strcpy(comp, token);
  comp_d = st_get(symbols, comp);
  if (!comp_d) {
    EXIT_ERROR("Invalid instruction");
  }
  if (*dest) {
    dest_d = st_get(symbols, dest);
    if (!dest_d) {
      EXIT_ERROR("Invalid instruction");
    }
  }
  if (*jmp) {
    jmp_d = st_get(symbols, jmp);
    if (!jmp_d) {
      EXIT_ERROR("Invalid instruction");
    }
  }
  fprintf(output, "%s%07b%03b%03b\n", opcode, comp_d, dest_d, jmp_d);
}
~~~

## Virtual Machine translator

### Overview

Jack VM is implemented using Stack Machine model. Arithmetic commands pop their operands from the top of the stack and push their results back onto the top of the stack. Other commands transfer data items from the stack’s top to designated memory locations, and vice versa. Thus the stack implements a last-in-ﬁrst-out (LIFO) storage model.

A complete VM program is organized in program units called functions, written in the VM language. Each function has its own stand-alone code and is separately handled. The VM language has a single 16-bit data type that can be used as an integer, a Boolean, or a pointer. The language consists of four types of commands:

- Arithmetic commands perform arithmetic and logical operations on the stack.
- Memory access commands transfer data between the stack and virtual memory segments.
- Program ﬂow commands facilitate conditional and unconditional branching operations.
- Function calling commands call functions and return from them.

### Parser

Every Jack VM program consisting of more than one file requires a bootstrap subroutine as an entry point. 

~~~c
void sys_init(FILE *output, char const *fname) {
  fprintf(output, "// %s\n\n", fname);
  fprintf(output, "// [0] Bootstrap Sys.init\n");
  fprintf(output, "\t@%u\n", STACK_ADDRESS);
  fprintf(output, "\tD=A\n");
  fprintf(output, "\t@SP\n");
  fprintf(output, "\tM=D\n");
}
~~~

Then we can simply go through each file line-by-line, tokenize input and translate it to Hack assembly language.

~~~c
void parse_file(FILE *file, FILE *ofile, char const *fname,
                unsigned *commandNumber) {
  fprintf(ofile, "// %s\n", fname);
  char line[MAX_LINE_LENGTH];
  char foo_name[MAX_SYMBOL_LENGTH];
  for (size_t lineNumber = 1; fgets(line, sizeof(line), file); lineNumber++) {
    char command[MAX_SYMBOL_LENGTH] = {0};
    char arg1[MAX_SYMBOL_LENGTH * 2] = "";
    char arg2[MAX_SYMBOL_LENGTH] = {0};
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
~~~

An example of a `write_command` for handling function declaration: 

~~~c
strcpy(foo_name, arg1);
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
~~~


## Compiler

### Syntax analyzer

Our compiler will have two stages. In the first one we need to understand the syntax of the program in order to uncover its semantics. This information enables us to reconstruct the program’s logic using the syntax of the target language. The syntax analysis task is divided further into two steps: tokenizing, or grouping of input characters into language atoms, and parsing, or attempting to match the resulting atoms stream to the syntax rules of the underlying language. 

First we create a linked list to store the underlying Jack program as a series of tokens along with their type and location in the source file.

~~~c
typedef enum { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STR_CONST } TokenType;

typedef enum { CLASS, CONSTRUCTOR, FUNCTION, METHOD, INT, BOOLEAN, CHAR, VOID, 
               VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, 
               FALSE, NUL, THIS } Keyword;

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

TokenList *tokenize_file(FILE *file) {
  TokenList *tl = malloc(sizeof(TokenList));
  if (tl == NULL) {
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
~~~
 
### Code writer

Now that we have a list of tokens we need to create a compile subroutine for every grammar rule in the Jack language. We will start with the first token in the list, try to identify whether it belongs to a statement, an expression, a declaration and so on and recursively compile it to VM bytecode. 

|||
|---|---|
| Lexical elements: | The Jack language includes ﬁve types of terminal elements (tokens): |
| keyword: | 'class' &#124; 'constructor' &#124; 'function' &#124; 'method' &#124; 'field' &#124; 'static' &#124; 'var' &#124; 'int' &#124; 'char' &#124; 'boolean' &#124; 'void' &#124; 'true' &#124; 'false' &#124; 'null' &#124; 'this' &#124; 'let' &#124; 'do' &#124; 'if' &#124; 'else' &#124; 'while' &#124; 'return' |
| symbol:| '{' &#124; '}' &#124; '(' &#124; ')' &#124; '[' &#124; ']' &#124; '.' &#124; ',' &#124; ';' &#124; '+' &#124; '-' &#124; '*' &#124; '/' &#124; '&' &#124;  '&#124;' &#124; '<' &#124; '>' &#124; '=' &#124; '~' |
| integerConstant: | A decimal number in the range 0 .. 32767. |
| StringConstant: | '" ' A sequence of Unicode characters not including double quote or newline '" ' |
| identiﬁer: |A sequence of letters, digits, and underscore ('_') not starting with a digit. |
|||
| Program structure: | A Jack program is a collection of classes, each appearing in a separate ﬁle. The compilation unit is a class. A class is a sequence of tokens structured according to the following context free syntax: |
| class: | 'class' className '{' classVarDec* subroutineDec* '}' |
| classVarDec: | ('static' | 'field') type varName (',' varName)* ';' |
| type: | 'int' &#124; 'char' &#124; 'boolean' &#124; className |
| subroutineDec: | ('constructor' &#124; 'function' &#124; 'method') ('void' &#124; type) subroutineName '(' parameterList ')' subroutineBody |
| parameterList: | ((type varName) (',' type varName)*)? |
| subroutineBody: | '{' varDec* statements '}' |
| varDec: | 'var' type varName (',' varName)* ';' |
| className: | identiﬁer |
| subroutineName: | identiﬁer |
| varName: | identiﬁer |
|||
| Statements: ||
| statements: | statement* |
| statement: | letStatement &#124; ifStatement &#124; whileStatement &#124; &#124; doStatement &#124; returnStatement |
| letStatement: | 'let' varName ('[' expression ']')? '=' expression ';' |
| ifStatement: | 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')? |
| whileStatement: | 'while' '(' expression ')' '{' statements '}' |
| doStatement: | 'do' subroutineCall ';' |
| ReturnStatement: | 'return' expression? ';' |
| Expressions: ||
| expression: | term (op term)* |
| term: | integerConstant &#124; stringConstant &#124; keywordConstant &#124; varName &#124; varName '[' expression ']' &#124; subroutineCall &#124; '(' expression ')' &#124; unaryOp term |
| subroutineCall: | subroutineName '(' expressionList ')' &#124; (className &#124; varName) '.' subroutineName '(' expressionList ')' |
| expressionList: | (expression (',' expression)* )? |
| op: | '+' &#124; '-' &#124; '*' &#124; '/' &#124; '&' &#124; '&#124;' &#124; '<' &#124; '>' &#124; '=' |
| unaryOp: | '-' &#124; '~' |
| KeywordConstant: | 'true' &#124; 'false' &#124; 'null' &#124; 'this' |

~~~c
TokenList *tl = tokenize_file(in);
t = tl->head;
compClass();

void compClass() {
  //'class' className '{' classVarDec* subroutineDec* '}'
  cst = st_new();
  if (cst == NULL) {
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
~~~


