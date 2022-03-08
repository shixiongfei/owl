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

#include <stdbool.h>
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

#ifdef _MSC_VER
#define P8_INLINE static __forceinline
#else
#define P8_INLINE static inline
#endif

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

typedef struct SDL_Surface p8_Image;
typedef struct SDL_Texture p8_Canvas;

P8_INLINE p8_Pixel p8_rgb(u8 r, u8 g, u8 b) {
  p8_Pixel p = {r, g, b, 0xFF};
  return p;
}

P8_INLINE p8_Pixel p8_rgba(u8 r, u8 g, u8 b, u8 a) {
  p8_Pixel p = {r, g, b, a};
  return p;
}

P8_API bool p8_init(s16 w, s16 h, const char *title, s32 flags);
P8_API void p8_quit(void);
P8_API bool p8_closed(void);
P8_API void p8_events(void);
P8_API void p8_update(p8_Canvas *screen);

P8_API bool p8_setfps(u8 fps);
P8_API u8 p8_getfps(void);
P8_API u32 p8_wait(void);

P8_API p8_Canvas *p8_canvas(s16 w, s16 h);
P8_API void p8_destroy(p8_Canvas *canvas);

P8_API void p8_clear(p8_Canvas *canvas, p8_Pixel color);
P8_API void p8_pixel(p8_Canvas *canvas, s16 x, s16 y, p8_Pixel color);

P8_API void p8_line(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
                    p8_Pixel color);
P8_API void p8_aaline(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
                      p8_Pixel color);
P8_API void p8_thickline(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
                         u8 width, p8_Pixel color);

P8_API void p8_arc(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start, s16 end,
                   p8_Pixel color);

P8_API void p8_rect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h,
                    p8_Pixel color);
P8_API void p8_fillrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h,
                        p8_Pixel color);

P8_API void p8_roundedrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h,
                           s16 rad, p8_Pixel color);
P8_API void p8_fillroundedrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h,
                               s16 rad, p8_Pixel color);

P8_API void p8_circle(p8_Canvas *canvas, s16 x, s16 y, s16 rad, p8_Pixel color);
P8_API void p8_aacircle(p8_Canvas *canvas, s16 x, s16 y, s16 rad,
                        p8_Pixel color);
P8_API void p8_fillcircle(p8_Canvas *canvas, s16 x, s16 y, s16 rad,
                          p8_Pixel color);

P8_API void p8_ellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                       p8_Pixel color);
P8_API void p8_aaellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                         p8_Pixel color);
P8_API void p8_fillellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                           p8_Pixel color);

P8_API void p8_pie(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start, s16 end,
                   p8_Pixel color);
P8_API void p8_fillpie(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start,
                       s16 end, p8_Pixel color);

P8_API void p8_trigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2, s16 x3,
                      s16 y3, p8_Pixel color);
P8_API void p8_aatrigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
                        s16 x3, s16 y3, p8_Pixel color);
P8_API void p8_filltrigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
                          s16 x3, s16 y3, p8_Pixel color);

P8_API void p8_polygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                       p8_Pixel color);
P8_API void p8_aapolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                         p8_Pixel color);
P8_API void p8_fillpolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy,
                           s32 n, p8_Pixel color);

P8_API void p8_bezier(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                      s32 s, p8_Pixel color);

P8_API void p8_imagepolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy,
                            s32 n, p8_Image *image, s32 dx, s32 dy);

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
