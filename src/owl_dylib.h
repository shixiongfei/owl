/*
 * owl_dylib.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_DYLIB_H__
#define __OWL_DYLIB_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *owl_Dylib;

extern owl_Dylib owl_dylib(const char *sofile);
extern void *owl_dysym(owl_Dylib dylib, const char *name);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_DYLIB_H__ */
