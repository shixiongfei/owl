/*
 * owlvm.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWLVM_H__
#define __OWLVM_H__

#include "owl.h"

#define OWLVM_MAJOR 0
#define OWLVM_MINOR 1
#define OWLVM_PATCH 0

#define OWLVM_VERMAJOR OWL_STR(OWLVM_MAJOR)
#define OWLVM_VERMINOR OWL_STR(OWLVM_MINOR)
#define OWLVM_VERPATCH OWL_STR(OWLVM_PATCH)

#define OWLVM_VERNUM ((OWLVM_MAJOR * 100) + OWLVM_MINOR)
#define OWLVM_VERFULL ((OWLVM_VERNUM * 100) + OWLVM_PATCH)
#define OWLVM_VERSION OWLVM_VERMAJOR "." OWLVM_VERMINOR
#define OWLVM_RELEASE OWLVM_VERSION "." OWLVM_VERPATCH

#ifdef __cplusplus
extern "C" {
#endif

OWL_API const char *owlvm_version(s32 *major, s32 *minor, s32 *patch);

#ifdef __cplusplus
};
#endif

#endif /* __OWLVM_H__ */
