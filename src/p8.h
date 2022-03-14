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

#include "p8_keys.h"

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

typedef float f32;
typedef double f64;

typedef intptr_t sword_t;
typedef uintptr_t uword_t;

typedef union p8_Pixel {
  struct {
    u8 r, g, b, a;
  };
  u32 rgba;
} p8_Pixel;

typedef struct p8_Point {
  s32 x, y;
} p8_Point;

typedef struct p8_Rect {
  s32 x, y;
  s32 w, h;
} p8_Rect;

#define P8_FORMAT_RGB 3
#define P8_FORMAT_RGBA 4

#define P8_FLIP_NONE 0
#define P8_FLIP_HORIZONTAL 1
#define P8_FLIP_VERTICAL 2

typedef struct SDL_Texture p8_Canvas;
typedef struct p8_Table p8_Table;
typedef void (*p8_Dtor)(void *);
typedef void (*p8_Events)();

P8_INLINE p8_Pixel p8_rgb(u8 r, u8 g, u8 b) {
  p8_Pixel p = {r, g, b, 0xFF};
  return p;
}

P8_INLINE p8_Pixel p8_rgba(u8 r, u8 g, u8 b, u8 a) {
  p8_Pixel p = {r, g, b, a};
  return p;
}

P8_API u64 p8_ticks(void);
P8_API void p8_sleep(u32 ms);

P8_API bool p8_init(s32 w, s32 h, const char *title, s32 flags);
P8_API void p8_quit(void);
P8_API bool p8_closed(void);
P8_API void p8_events(p8_Events cb);
P8_API void p8_present(p8_Canvas *screen);

P8_API bool p8_setfps(u32 rate);
P8_API u32 p8_getfps(void);
P8_API u32 p8_wait(void);

P8_API u32 p8_mouse(s32 *x, s32 *y);
P8_API bool p8_pressed(u32 key);

P8_API p8_Canvas *p8_canvas(s32 w, s32 h);
P8_API p8_Canvas *p8_image(const u8 *data, s32 w, s32 h, s32 format);
P8_API p8_Canvas *p8_imagex(const u8 *data, s32 w, s32 h, p8_Pixel colorkey);
P8_API p8_Canvas *p8_load(const char *filename);
P8_API p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey);
P8_API void p8_destroy(p8_Canvas *canvas);

P8_API void p8_size(p8_Canvas *canvas, s32 *w, s32 *h);

P8_API void p8_clear(p8_Canvas *canvas);
P8_API void p8_color(p8_Canvas *canvas, p8_Pixel color);

P8_API void p8_pixel(p8_Canvas *canvas, s32 x, s32 y);
P8_API void p8_pixels(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_line(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2);
P8_API void p8_lines(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_rect(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_rects(p8_Canvas *canvas, const p8_Rect *rects, s32 n);

P8_API void p8_fillrect(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_fillrects(p8_Canvas *canvas, const p8_Rect *rects, s32 n);

P8_API void p8_clip(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_blit(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
                    const p8_Rect *dstrect);
P8_API void p8_blitex(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
                      const p8_Rect *dstrect, f64 angle, const p8_Point *center,
                      u32 flip);

P8_API p8_Table *p8_table(void);
P8_API void p8_tablefree(p8_Table *table, p8_Dtor dtor);
P8_API void p8_cleartable(p8_Table *table, p8_Dtor dtor);
P8_API s32 p8_tablesize(p8_Table *table);
P8_API void *p8_settable(p8_Table *table, const char *name, void *value);
P8_API void *p8_gettable(p8_Table *table, const char *name);
P8_API void *p8_isettable(p8_Table *table, u64 key, void *value);
P8_API void *p8_igettable(p8_Table *table, u64 key);

P8_API bool p8_loadfont(const char *name, const char *filename);
P8_API bool p8_font(const char *name, s32 size);
P8_API void p8_text(p8_Canvas *canvas, const char *text, s32 x, s32 y,
                    p8_Pixel color);
P8_API s32 p8_textwidth(const char *text);

P8_API bool p8_loadsound(const char *name, const char *filename);
P8_API bool p8_playing(const char *name);
P8_API bool p8_play(const char *name);
P8_API bool p8_stop(const char *name);
P8_API bool p8_pause(const char *name);
P8_API bool p8_resume(const char *name);

P8_API s64 p8_filesize(const char *filename);
P8_API u8 *p8_readfile(const char *filename);

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
