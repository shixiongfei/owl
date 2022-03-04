/*
 * p8.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#ifndef __P8_H__
#define __P8_H__

#include <stddef.h>

#define P8_MAJOR 0
#define P8_MINOR 1
#define P8_PATCH 0

#define P8__STR(x) #x
#define P8_STR(x) P8__STR(x)

#define P8_VERMAJOR P8_STR(P8_MAJOR)
#define P8_VERMINOR P8_STR(P8_MINOR)
#define P8_VERPATCH P8_STR(P8_PATCH)

#define P8_VERNUM ((P8_MAJOR * 100) + P8_MINOR)
#define P8_VERFULL ((P8_VERNUM * 100) + P8_PATCH)
#define P8_VERSION P8_VERMAJOR "." P8_VERMINOR
#define P8_RELEASE P8_VERSION "." P8_VERPATCH

#ifndef _WIN32
#define P8_EXPORT __attribute__((visibility("default")))
#define P8_IMPORT __attribute__((visibility("default")))
#else
#define P8_EXPORT __declspec(dllexport)
#define P8_IMPORT __declspec(dllimport)
#endif

#if defined(P8_STATIC)
#define P8_API extern
#elif defined(P8_BUILD_DLL)
#define P8_API P8_EXPORT
#else
#define P8_API P8_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

P8_API const char *p8_version();

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
