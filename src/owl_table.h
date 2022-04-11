/*
 * owl_table.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_TABLE_H__
#define __OWL_TABLE_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct owl_Table owl_Table;

extern owl_Table *owl_table(void);
extern void owl_freetable(owl_Table *table, owl_Dtor dtor);
extern void owl_cleartable(owl_Table *table, owl_Dtor dtor);
extern s32 owl_tablesize(owl_Table *table);
extern void *owl_settable(owl_Table *table, const char *name, void *value);
extern void *owl_gettable(owl_Table *table, const char *name);
extern void *owl_isettable(owl_Table *table, u64 key, void *value);
extern void *owl_igettable(owl_Table *table, u64 key);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_TABLE_H__ */
