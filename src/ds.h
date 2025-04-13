//
#ifndef DS_H
#define DS_H

#include <stdbool.h>
#include <stddef.h>

// hash-table {{{1
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
ht *ht_new(void);
void ht_dump(ht *self);
void ht_del(ht *self);
unsigned ht_get(ht *self, const char *key);
const char *ht_set(ht *self, const char *key, unsigned value);
size_t ht_len(ht *self);
ht_i ht_iter(ht *self);
bool ht_next(ht_i *it);
// }}}1
#endif
