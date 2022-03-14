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

typedef struct p8_TrieNode {
  void *value;
  struct p8_TrieNode *next[P8_TRIE_FACTOR];
} p8_TrieNode;

struct p8_Table {
  s32 count;
  p8_TrieNode *root;
};

#define p8_trie() (p8_TrieNode *)calloc(1, sizeof(p8_TrieNode))

static void p8_triefree(p8_TrieNode *trie) {
  s32 i;

  for (i = 0; i < P8_TRIE_FACTOR; ++i)
    if (trie->next[i]) {
      p8_triefree(trie->next[i]);
      trie->next[i] = NULL;
    }

  free(trie);
}

static void p8_triedtor(p8_Table *table, p8_TrieNode *trie, p8_Dtor dtor) {
  s32 i;

  if (trie->value) {
    if (dtor)
      dtor(trie->value);

    trie->value = NULL;
    table->count -= 1;
  }

  for (i = 0; i < P8_TRIE_FACTOR; ++i)
    if (trie->next[i])
      p8_triedtor(table, trie->next[i], dtor);
}

static void *p8_settrie(p8_Table *table, p8_TrieNode *trie, void *value) {
  void *oldval = trie->value;

  if (!oldval)
    table->count += 1;

  trie->value = value;

  if (!value)
    table->count -= 1;

  return oldval;
}

static p8_TrieNode **p8_gettrie(p8_TrieNode **root, const char *name,
                                bool build) {
  p8_TrieNode **node = root;
  const u8 *p = (const u8 *)name;
  u8 ch, lo, hi;

  if (!name)
    return NULL;

  while (!!(ch = *p++)) {
    lo = ch & P8_TRIE_MASK;
    hi = ch >> P8_TRIE_BITS;

    node = &(*node)->next[lo];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = p8_trie();
    }

    node = &(*node)->next[hi];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = p8_trie();
    }
  }
  return node;
}

static p8_TrieNode **p8_igettrie(p8_TrieNode **root, u64 key, bool build) {
  p8_TrieNode **node = root;

  while (key > 0) {
    node = &(*node)->next[key & P8_TRIE_MASK];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = p8_trie();
    }

    key = key >> P8_TRIE_BITS;
  }
  return node;
}

p8_Table *p8_table(void) {
  p8_Table *table = (p8_Table *)malloc(sizeof(p8_Table));

  if (!table)
    return NULL;

  table->count = 0;
  table->root = p8_trie();

  return table;
}

void p8_freetable(p8_Table *table, p8_Dtor dtor) {
  p8_triedtor(table, table->root, dtor);
  p8_triefree(table->root);
  free(table);
}

void p8_cleartable(p8_Table *table, p8_Dtor dtor) {
  p8_TrieNode *root = table->root;
  s32 i;

  p8_triedtor(table, root, dtor);

  for (i = 0; i < P8_TRIE_FACTOR; ++i)
    if (root->next[i]) {
      p8_triefree(root->next[i]);
      root->next[i] = NULL;
    }
}

s32 p8_tablesize(p8_Table *table) { return table->count; }

void *p8_settable(p8_Table *table, const char *name, void *value) {
  p8_TrieNode **node = p8_gettrie(&table->root, name, true);

  if (!node)
    return NULL;

  return p8_settrie(table, *node, value);
}

void *p8_gettable(p8_Table *table, const char *name) {
  p8_TrieNode **node = p8_gettrie(&table->root, name, false);

  if (!node)
    return NULL;

  return (*node)->value;
}

void *p8_isettable(p8_Table *table, u64 key, void *value) {
  p8_TrieNode **node = p8_igettrie(&table->root, key, true);

  if (!node)
    return NULL;

  return p8_settrie(table, *node, value);
}

void *p8_igettable(p8_Table *table, u64 key) {
  p8_TrieNode **node = p8_igettrie(&table->root, key, false);

  if (!node)
    return NULL;

  return (*node)->value;
}
