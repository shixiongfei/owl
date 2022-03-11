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

#include "p8.h"

#define P8_TRIE_BITS 4
#define P8_TRIE_FACTOR (1 << P8_TRIE_BITS)
#define P8_TRIE_MASK (P8_TRIE_FACTOR - 1)

struct p8_Table {
  p8_Table *next[P8_TRIE_FACTOR];
};

p8_Table *p8_table(void) { return NULL; }

void p8_tablefree(p8_Table *table) {}

void *p8_settable(p8_Table *table, const char *name, void *value) {
  return NULL;
}

void *p8_gettable(p8_Table *table, const char *name) {
  return NULL;
}

void p8_cleartable(p8_Table *table, void (*dtor)(void *)) {}
