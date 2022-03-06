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
#include <stdint.h>

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

#define P8_API extern

#if defined(_WIN32)
#define P8_WINDOWS 1
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IOS
#define P8_IOS 1
#elif TARGET_OS_MAC
#define P8_MACOS 1
#else
#error "Unsupported Platform"
#endif
#elif defined(__ANDROID__)
#define P8_ANDROID 1
#elif defined(__linux__)
#define P8_LINUX 1
#elif defined(__BSD__)
#define P8_BSD 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef intptr_t sword_t;
typedef uintptr_t uword_t;

typedef union p8_Pixel {
  struct {
    u8 r, g, b, a;
  };
  u32 rgba;
} p8_Pixel;

typedef struct p8_Canvas p8_Canvas;

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
