//
// definitions {{{1
#include "ds.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hash-table
#define INITIAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// hash-table {{{1
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
      printf("%d\t%016b\t%s\n", self->entries[i].value, self->entries[i].value,
             self->entries[i].key);
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
// ht_hash {{{2
static uint64_t ht_hash(const char *key) {
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}
// ht_get {{{2
unsigned ht_get(ht *self, const char *key) {
  uint64_t hash = ht_hash(key);
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
// ht_set_entry {{{2
static const char *ht_set_entry(ht_entry *entries, size_t capacity,
                                const char *key, unsigned value,
                                size_t *length_ptr) {
  uint64_t hash = ht_hash(key);
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
// ht_expand {{{2
static bool ht_expand(ht *self) {
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
      ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
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
    if (!ht_expand(self)) {
      return NULL;
    }
  }
  return ht_set_entry(self->entries, self->capacity, key, value, &self->length);
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
