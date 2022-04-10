/*
 * owl_table.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include <stdlib.h>

#include "owl.h"

#define OWL_TRIE_BITS 4
#define OWL_TRIE_FACTOR (1 << OWL_TRIE_BITS)
#define OWL_TRIE_MASK (OWL_TRIE_FACTOR - 1)

typedef struct owl_TrieNode {
  void *value;
  struct owl_TrieNode *next[OWL_TRIE_FACTOR];
} owl_TrieNode;

struct owl_Table {
  s32 count;
  owl_TrieNode *root;
};

#define owl_trie() (owl_TrieNode *)calloc(1, sizeof(owl_TrieNode))

static void owl_triefree(owl_TrieNode *trie) {
  s32 i;

  for (i = 0; i < OWL_TRIE_FACTOR; ++i)
    if (trie->next[i]) {
      owl_triefree(trie->next[i]);
      trie->next[i] = NULL;
    }

  free(trie);
}

static void owl_triedtor(owl_Table *table, owl_TrieNode *trie, owl_Dtor dtor) {
  s32 i;

  if (trie->value) {
    if (dtor)
      dtor(trie->value);

    trie->value = NULL;
    table->count -= 1;
  }

  for (i = 0; i < OWL_TRIE_FACTOR; ++i)
    if (trie->next[i])
      owl_triedtor(table, trie->next[i], dtor);
}

static void *owl_settrie(owl_Table *table, owl_TrieNode *trie, void *value) {
  void *oldval = trie->value;

  if (!oldval)
    table->count += 1;

  trie->value = value;

  if (!value)
    table->count -= 1;

  return oldval;
}

static owl_TrieNode **owl_gettrie(owl_TrieNode **root, const char *name,
                                  bool build) {
  owl_TrieNode **node = root;
  const u8 *p = (const u8 *)name;
  u8 ch, lo, hi;

  if (!name)
    return NULL;

  while (!!(ch = *p++)) {
    lo = ch & OWL_TRIE_MASK;
    hi = ch >> OWL_TRIE_BITS;

    node = &(*node)->next[lo];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = owl_trie();
    }

    node = &(*node)->next[hi];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = owl_trie();
    }
  }
  return node;
}

static owl_TrieNode **owl_igettrie(owl_TrieNode **root, u64 key, bool build) {
  owl_TrieNode **node = root;

  while (key > 0) {
    node = &(*node)->next[key & OWL_TRIE_MASK];

    if (!(*node)) {
      if (!build)
        return NULL;

      *node = owl_trie();
    }

    key = key >> OWL_TRIE_BITS;
  }
  return node;
}

owl_Table *owl_table(void) {
  owl_Table *table = (owl_Table *)malloc(sizeof(owl_Table));

  if (!table)
    return NULL;

  table->count = 0;
  table->root = owl_trie();

  return table;
}

void owl_freetable(owl_Table *table, owl_Dtor dtor) {
  owl_triedtor(table, table->root, dtor);
  owl_triefree(table->root);
  free(table);
}

void owl_cleartable(owl_Table *table, owl_Dtor dtor) {
  owl_TrieNode *root = table->root;
  s32 i;

  owl_triedtor(table, root, dtor);

  for (i = 0; i < OWL_TRIE_FACTOR; ++i)
    if (root->next[i]) {
      owl_triefree(root->next[i]);
      root->next[i] = NULL;
    }
}

s32 owl_tablesize(owl_Table *table) { return table->count; }

void *owl_settable(owl_Table *table, const char *name, void *value) {
  owl_TrieNode **node = owl_gettrie(&table->root, name, true);

  if (!node)
    return NULL;

  return owl_settrie(table, *node, value);
}

void *owl_gettable(owl_Table *table, const char *name) {
  owl_TrieNode **node = owl_gettrie(&table->root, name, false);

  if (!node)
    return NULL;

  return (*node)->value;
}

void *owl_isettable(owl_Table *table, u64 key, void *value) {
  owl_TrieNode **node = owl_igettrie(&table->root, key, true);

  if (!node)
    return NULL;

  return owl_settrie(table, *node, value);
}

void *owl_igettable(owl_Table *table, u64 key) {
  owl_TrieNode **node = owl_igettrie(&table->root, key, false);

  if (!node)
    return NULL;

  return (*node)->value;
}
