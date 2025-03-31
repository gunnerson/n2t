// definitions {{{1

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128
#define START_SYMBOL_ADDRESS 1024U
#define N_BUCKETS 16U

// symbol key-value store {{{1

struct SymbolTableEntry {
  char *symbol;
  unsigned address;
  struct SymbolTableEntry *__next;
};

struct SymbolTable {
  int __buckets;
  struct SymbolTableEntry *__heads[N_BUCKETS];
  struct SymbolTableEntry *__tails[N_BUCKETS];

  void (*addEntry)(struct SymbolTable *table, char *symbol, unsigned address);
  unsigned (*getAddress)(struct SymbolTable *table, char *symbol);
};

int getBucket(char *symbol, int buckets) {
  unsigned hash = 0;
  for (; *symbol;) {
    hash = *symbol++ + (hash << 6) + (hash << 16) - hash;
  }
  return hash % buckets;
}

struct SymbolTableEntry *__SymbolTable_find(struct SymbolTable *table,
                                            char *symbol, unsigned bucket) {
  struct SymbolTableEntry *cur;
  for (cur = table->__heads[bucket]; cur != NULL; cur = cur->__next) {
    if (strcmp(symbol, cur->symbol) == 0)
      return cur;
  }
  return NULL;
}

void __SymbolTable_add(struct SymbolTable *table, char *symbol,
                       unsigned address) {
  int bucket;
  struct SymbolTableEntry *old, *new;
  char *new_symbol;
  bucket = getBucket(symbol, table->__buckets);
  old = __SymbolTable_find(table, symbol, bucket);
  if (old) {
    old->address = address;
    return;
  }
  new = malloc(sizeof(*new));
  new->__next = NULL;
  if (table->__tails[bucket])
    table->__tails[bucket]->__next = new;
  else
    table->__heads[bucket] = new;
  table->__tails[bucket] = new;
  new->symbol = malloc(strlen(symbol) + 1);
  strcpy(new->symbol, symbol);
  new->address = address;
}

unsigned __SymbolTable_get(struct SymbolTable *table, char *symbol) {
  int bucket = getBucket(symbol, table->__buckets);
  struct SymbolTableEntry *retval = __SymbolTable_find(table, symbol, bucket);
  if (retval == NULL)
    return 0;
  return retval->address;
}

struct SymbolTable *SymbolTable_new() {
  struct SymbolTable *p = malloc(sizeof(*p));
  p->__buckets = N_BUCKETS;
  for (int i = 0; i < p->__buckets; i++) {
    p->__heads[i] = NULL;
    p->__tails[i] = NULL;
  }
  p->addEntry = &__SymbolTable_add;
  p->getAddress = &__SymbolTable_get;
  return p;
}

// addrToStr {{{1
void addrToStr(char *str, int num, int bits) {
  int num_bits = sizeof(int) * 8;
  char *tmp = malloc(num_bits + 1);
  for (int i = num_bits - 1; i >= 0; i--) {
    tmp[i] = (num & 1) + '0';
    num >>= 1;
  }
  strcpy(str, tmp + num_bits - bits);
}

// main {{{1
int main(int argc, char *argv[]) {
  // handle file {{{2
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // initialize symbols table {{{2
  struct SymbolTable *symbols = SymbolTable_new();
  symbols->addEntry(symbols, "SP", 0);
  symbols->addEntry(symbols, "LCL", 1);
  symbols->addEntry(symbols, "ARG", 2);
  symbols->addEntry(symbols, "THIS", 3);
  symbols->addEntry(symbols, "THAT", 4);
  symbols->addEntry(symbols, "R0", 0);
  symbols->addEntry(symbols, "R1", 1);
  symbols->addEntry(symbols, "R2", 2);
  symbols->addEntry(symbols, "R3", 3);
  symbols->addEntry(symbols, "R4", 4);
  symbols->addEntry(symbols, "R5", 5);
  symbols->addEntry(symbols, "R6", 6);
  symbols->addEntry(symbols, "R7", 7);
  symbols->addEntry(symbols, "R8", 8);
  symbols->addEntry(symbols, "R9", 9);
  symbols->addEntry(symbols, "R10", 10);
  symbols->addEntry(symbols, "R11", 11);
  symbols->addEntry(symbols, "R12", 12);
  symbols->addEntry(symbols, "R13", 13);
  symbols->addEntry(symbols, "R14", 14);
  symbols->addEntry(symbols, "R15", 15);
  symbols->addEntry(symbols, "SCREEN", 16384);
  symbols->addEntry(symbols, "KBD", 24576);

  symbols->addEntry(symbols, "0", 42);
  symbols->addEntry(symbols, "1", 63);
  symbols->addEntry(symbols, "-1", 58);
  symbols->addEntry(symbols, "D", 12);
  symbols->addEntry(symbols, "A", 48);
  symbols->addEntry(symbols, "!D", 13);
  symbols->addEntry(symbols, "!A", 49);
  symbols->addEntry(symbols, "-D", 15);
  symbols->addEntry(symbols, "-A", 51);
  symbols->addEntry(symbols, "D+1", 31);
  symbols->addEntry(symbols, "A+1", 55);
  symbols->addEntry(symbols, "D-1", 14);
  symbols->addEntry(symbols, "A-1", 50);
  symbols->addEntry(symbols, "D+A", 2);
  symbols->addEntry(symbols, "D-A", 19);
  symbols->addEntry(symbols, "A-D", 7);
  symbols->addEntry(symbols, "D&A", 0);
  symbols->addEntry(symbols, "D|A", 21);
  symbols->addEntry(symbols, "M", 112);
  symbols->addEntry(symbols, "!M", 113);
  symbols->addEntry(symbols, "-M", 115);
  symbols->addEntry(symbols, "M+1", 119);
  symbols->addEntry(symbols, "M-1", 114);
  symbols->addEntry(symbols, "D+M", 66);
  symbols->addEntry(symbols, "D-M", 83);
  symbols->addEntry(symbols, "M-D", 71);
  symbols->addEntry(symbols, "D&M", 64);
  symbols->addEntry(symbols, "D|M", 85);

  symbols->addEntry(symbols, "@M", 1);
  symbols->addEntry(symbols, "@D", 2);
  symbols->addEntry(symbols, "MD", 3);
  symbols->addEntry(symbols, "@A", 4);
  symbols->addEntry(symbols, "AM", 5);
  symbols->addEntry(symbols, "AD", 6);
  symbols->addEntry(symbols, "AMD", 7);

  symbols->addEntry(symbols, "JGT", 1);
  symbols->addEntry(symbols, "JEQ", 2);
  symbols->addEntry(symbols, "JGE", 3);
  symbols->addEntry(symbols, "JLT", 4);
  symbols->addEntry(symbols, "JNE", 5);
  symbols->addEntry(symbols, "JLE", 6);
  symbols->addEntry(symbols, "JMP", 7);

  // first pass {{{2

  char line[MAX_LINE_LENGTH];
  int nextAddress = START_SYMBOL_ADDRESS;
  int j, lineNumber = 0;
  bool isVar, isAInstr, isLabel, isInstr;

  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    char symbol[MAX_LINE_LENGTH] = {0};
    j = 0;
    isVar = false;
    isAInstr = false;
    isLabel = false;
    isInstr = false;

    for (int i = 0; i < strlen(line); i++) {
      char c = line[i];

      if (isVar || isLabel) {
        if (c == ' ' || c == '\r' || c == '\n' || c == ')') {
          break;
        } else {
          symbol[j++] = c;
          continue;
        }
      }

      if (isAInstr) {
        if (isdigit(c)) {
          isAInstr = false;
          break;
        } else {
          isVar = true;
          symbol[j++] = c;
          continue;
        }
      }

      if (c == '/')
        break;
      if (c == '\r' || c == '\n') {
        break;
      }
      if (c == ' ')
        continue;
      if (c == '@')
        isAInstr = true;
      else if (c == '(') {
        isLabel = true;
      }
      if (!isLabel)
        isInstr = true;
    }

    if (*symbol) {
      if (isLabel)
        symbols->addEntry(symbols, symbol, lineNumber);
      else if (strcmp(symbol, "SP") && strcmp(symbol, "R0") &&
               !symbols->getAddress(symbols, symbol)) {
        symbols->addEntry(symbols, symbol, nextAddress++);
      }
    }

    if (isInstr)
      lineNumber++;
  }

  // second pass {{{2
  unsigned d_addr;
  bool hasJump;
  rewind(file);
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    char aInstr[MAX_LINE_LENGTH] = {0};
    char addr[MAX_LINE_LENGTH] = {0};
    char tmp[MAX_LINE_LENGTH] = {0};
    char opcode[4] = {0};
    char dest[6] = {0};
    char comp[8] = {0};
    char jmp[4] = {0};
    j = 0;
    isAInstr = false;
    hasJump = false;

    for (int i = 0; i < strlen(line); i++) {
      char c = line[i];

      if (c == '/' || c == '\r' || c == '\n' || c == '(')
        break;
      if (!*opcode) {
        if (c == ' ')
          continue;
        if (c == '@') {
          strcpy(opcode, "0");
        } else {
          strcpy(opcode, "111");
          tmp[j++] = c;
        }

      } else {
        if (c == ' ')
          break;
        if (opcode[0] == '0') {
          aInstr[j++] = c;

        } else {
          if (c == '=') {
            if (!strcmp(tmp, "M") || !strcmp(tmp, "D") || !strcmp(tmp, "A")) {
              strcat(dest, "@");
              strcat(dest, tmp);
            } else
              strcpy(dest, tmp);
            j = 0;
          } else if (c == ';') {
            strcpy(comp, tmp);
            j = 0;
            hasJump = true;
          } else
            tmp[j++] = c;
        }
      }
    }

    if (opcode[0] == '0') {
      if (isdigit(aInstr[0])) {
        d_addr = atoi(aInstr);
      } else {
        d_addr = symbols->getAddress(symbols, aInstr);
      }
      addrToStr(aInstr, d_addr, 15);
      printf("%s%s\n", opcode, aInstr);

    } else if (opcode[0] == '1') {
      (hasJump) ? strcpy(jmp, tmp) : strcpy(comp, tmp);
      d_addr = symbols->getAddress(symbols, comp);
      addrToStr(comp, d_addr, 7);
      d_addr = symbols->getAddress(symbols, dest);
      addrToStr(dest, d_addr, 3);
      d_addr = symbols->getAddress(symbols, jmp);
      addrToStr(jmp, d_addr, 3);
      printf("%s%s%s%s\n", opcode, comp, dest, jmp);
    }
  }

  fclose(file);
  return 0;
}
