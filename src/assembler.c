// definitions {{{1
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128
#define MAX_TOKEN_LENGTH 4
#define START_SYMBOL_ADDRESS 16U
#define SCREEN_ADDRESS 16384U
#define KEYBOARD_ADDRESS 24576U
#define MAX_ADDRESS 32767U
#define INITIAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define EXIT_ERROR(t)                                                          \
  do {                                                                         \
    fprintf(stderr, "Error on line %zu: %s\n", lineNumber, t);                 \
    free(file_name);                                                           \
    free(full_file_name);                                                      \
    fclose(output);                                                            \
    fclose(file);                                                              \
    ht_del(symbols);                                                           \
    return EXIT_FAILURE;                                                       \
  } while (0)
// hash-table {{{1
// declarations {{{2
typedef struct {
  const char *key;
  unsigned value;
} ht_entry;
typedef struct {
  ht_entry *entries;
  size_t capacity;
  size_t length;
} ht;
typedef struct {
  const char *key;
  unsigned value;
  ht *_self;
  size_t _index;
} ht_i;
// ht_new {{{2
ht *ht_new(void) {
  ht *self = malloc(sizeof(ht));
  if (self == NULL) {
    perror("Failed to allocate memory!");
    return NULL;
  }
  self->length = 0;
  self->capacity = INITIAL_CAPACITY;
  self->entries = calloc(self->capacity, sizeof(ht_entry));
  if (self->entries == NULL) {
    perror("Failed to allocate memory!");
    free(self);
    return NULL;
  }
  return self;
}
// ht_dump {{{2
void ht_dump(ht *self) {
  for (size_t i = 0; i < self->capacity; i++) {
    if (self->entries[i].key) {
      printf("%s\t%d\t%016b\n", self->entries[i].key, self->entries[i].value,
             self->entries[i].value);
    }
  }
}
// ht_del {{{2
void ht_del(ht *self) {
  for (size_t i = 0; i < self->capacity; i++) {
    free((void *)self->entries[i].key);
  }
  free(self->entries);
  free(self);
}
// _hash {{{2
static uint64_t _hash(const char *key) {
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}
// ht_get {{{2
unsigned ht_get(ht *self, const char *key) {
  uint64_t hash = _hash(key);
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
// _ht_set_entry {{{2
static const char *_ht_set_entry(ht_entry *entries, size_t capacity,
                                 const char *key, unsigned value,
                                 size_t *length_ptr) {
  uint64_t hash = _hash(key);
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
      perror("Failed to allocate memory!");
      return NULL;
    }
    (*length_ptr)++;
  }
  entries[index].key = (char *)key;
  entries[index].value = value;
  return key;
}
// _ht_expand {{{2
static bool _ht_expand(ht *self) {
  size_t new_capacity = self->capacity * 2;
  if (new_capacity < self->capacity) {
    return false;
  }
  ht_entry *new_entries = calloc(new_capacity, sizeof(ht_entry));
  if (new_entries == NULL) {
    perror("Failed to allocate memory!");
    return false;
  }
  for (size_t i = 0; i < self->capacity; i++) {
    ht_entry entry = self->entries[i];
    if (entry.key != NULL) {
      _ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
    }
  }
  free(self->entries);
  self->entries = new_entries;
  self->capacity = new_capacity;
  return true;
}
// ht_set {{{2
const char *ht_set(ht *self, const char *key, unsigned value) {
  if (self->length >= self->capacity / 2) {
    if (!_ht_expand(self)) {
      return NULL;
    }
  }
  return _ht_set_entry(self->entries, self->capacity, key, value,
                       &self->length);
}
// ht_len {{{2
size_t ht_len(ht *self) { return self->length; }
// ht_iter {{{2
ht_i ht_iter(ht *self) {
  ht_i it;
  it._self = self;
  it._index = 0;
  return it;
}
// ht_next {{{2
bool ht_next(ht_i *it) {
  ht *self = it->_self;
  while (it->_index < self->capacity) {
    size_t i = it->_index++;
    if (self->entries[i].key != NULL) {
      ht_entry entry = self->entries[i];
      it->key = entry.key;
      it->value = entry.value;
      return true;
    }
  }
  return false;
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
    char *of = (char *)malloc(sizeof(char) * (strlen(full_file_name) + 6));
    snprintf(of, strlen(full_file_name) + 6, "%s.hack", full_file_name);
    output = fopen(of, "w");
    if (output == NULL) {
      perror("Error opening file");
      return EXIT_FAILURE;
    }
    free(of);
  }
  // initialize symbols table {{{2
  ht *symbols = ht_new();
  if (symbols == NULL) {
    perror("Failed to allocate memory!");
    return EXIT_FAILURE;
  }
  ht_set(symbols, "SP", 0);
  ht_set(symbols, "LCL", 1);
  ht_set(symbols, "ARG", 2);
  ht_set(symbols, "THIS", 3);
  ht_set(symbols, "THAT", 4);
  ht_set(symbols, "R0", 0);
  ht_set(symbols, "R1", 1);
  ht_set(symbols, "R2", 2);
  ht_set(symbols, "R3", 3);
  ht_set(symbols, "R4", 4);
  ht_set(symbols, "R5", 5);
  ht_set(symbols, "R6", 6);
  ht_set(symbols, "R7", 7);
  ht_set(symbols, "R8", 8);
  ht_set(symbols, "R9", 9);
  ht_set(symbols, "R10", 10);
  ht_set(symbols, "R11", 11);
  ht_set(symbols, "R12", 12);
  ht_set(symbols, "R13", 13);
  ht_set(symbols, "R14", 14);
  ht_set(symbols, "R15", 15);
  ht_set(symbols, "SCREEN", SCREEN_ADDRESS);
  ht_set(symbols, "KBD", KEYBOARD_ADDRESS);

  ht_set(symbols, "0", 42);
  ht_set(symbols, "1", 63);
  ht_set(symbols, "-1", 58);
  ht_set(symbols, "D", 12);
  ht_set(symbols, "A", 48);
  ht_set(symbols, "!D", 13);
  ht_set(symbols, "!A", 49);
  ht_set(symbols, "-D", 15);
  ht_set(symbols, "-A", 51);
  ht_set(symbols, "D+1", 31);
  ht_set(symbols, "A+1", 55);
  ht_set(symbols, "D-1", 14);
  ht_set(symbols, "A-1", 50);
  ht_set(symbols, "D+A", 2);
  ht_set(symbols, "D-A", 19);
  ht_set(symbols, "A-D", 7);
  ht_set(symbols, "D&A", 0);
  ht_set(symbols, "D|A", 21);
  ht_set(symbols, "M", 112);
  ht_set(symbols, "!M", 113);
  ht_set(symbols, "-M", 115);
  ht_set(symbols, "M+1", 119);
  ht_set(symbols, "M-1", 114);
  ht_set(symbols, "D+M", 66);
  ht_set(symbols, "D-M", 83);
  ht_set(symbols, "M-D", 71);
  ht_set(symbols, "D&M", 64);
  ht_set(symbols, "D|M", 85);

  ht_set(symbols, "@M", 1);
  ht_set(symbols, "@D", 2);
  ht_set(symbols, "MD", 3);
  ht_set(symbols, "@A", 4);
  ht_set(symbols, "AM", 5);
  ht_set(symbols, "AD", 6);
  ht_set(symbols, "AMD", 7);

  ht_set(symbols, "JGT", 1);
  ht_set(symbols, "JEQ", 2);
  ht_set(symbols, "JGE", 3);
  ht_set(symbols, "JLT", 4);
  ht_set(symbols, "JNE", 5);
  ht_set(symbols, "JLE", 6);
  ht_set(symbols, "JMP", 7);
  // first pass {{{2
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

      if (c == '/')
        break;
      if (c == '\r' || c == '\n' || c == '\t') {
        break;
      }
      if (c == ' ')
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
      ht_set(symbols, symbol, instrNumber);
    } else if (isInstr) {
      instrNumber++;
    }
  }
  // second pass {{{2
  rewind(file);
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

      if (c == '/' || c == '\r' || c == '\n' || c == '\t' || c == '(')
        break;
      if (!*opcode) {
        if (c == ' ')
          continue;
        if (c == '@') {
          strcpy(opcode, "0");
        } else {
          strcpy(opcode, "111");
          token[j++] = c;
        }

      } else {
        if (c == ' ')
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
          addr = ht_get(symbols, aInstr);
          if (!addr) {
            if (nextAddress >= SCREEN_ADDRESS) {
              EXIT_ERROR("Instruction address limit reached");
            }
            addr = nextAddress++;
            ht_set(symbols, aInstr, addr);
          }
        }
      }
      fprintf(output, "%s%015b\n", opcode, addr);

    } else if (opcode[0] == '1') {
      (hasJump) ? strcpy(jmp, token) : strcpy(comp, token);
      comp_d = ht_get(symbols, comp);
      if (!comp_d) {
        EXIT_ERROR("Invalid instruction");
      }
      if (*dest) {
        dest_d = ht_get(symbols, dest);
        if (!dest_d) {
          EXIT_ERROR("Invalid instruction");
        }
      }
      if (*jmp) {
        jmp_d = ht_get(symbols, jmp);
        if (!jmp_d) {
          EXIT_ERROR("Invalid instruction");
        }
      }
      fprintf(output, "%s%07b%03b%03b\n", opcode, comp_d, dest_d, jmp_d);
    }
  }
  // }}}2
  free(file_name);
  free(full_file_name);
  fclose(output);
  fclose(file);
  ht_del(symbols);
  return EXIT_SUCCESS;
}
