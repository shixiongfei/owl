/*
 * p8_table.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#include <stdlib.h>

#include "p8.h"

#define P8_TRIE_BITS 4
#define P8_TRIE_FACTOR (1 << P8_TRIE_BITS)
#define P8_TRIE_MASK (P8_TRIE_FACTOR - 1)

struct p8_Table {
  p8_Table *next[P8_TRIE_FACTOR];
};

p8_Table *p8_table(void) {
  p8_Table *table = (p8_Table *)calloc(1, sizeof(p8_Table));

  if (!table)
    return NULL;

  return table;
}

void p8_tablefree(p8_Table *table) {
  s32 i;

  for (i = 0; i < P8_TRIE_FACTOR; ++i)
    if (table->next[i])
      p8_tablefree(table->next[i]);

  free(table);
}

void *p8_settable(p8_Table *table, const char *name, void *value) {
  return NULL;
}

void *p8_gettable(p8_Table *table, const char *name) {
  return NULL;
}

void p8_cleartable(p8_Table *table, p8_Dtor dtor) {}
