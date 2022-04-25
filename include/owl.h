/*
 * owl.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_H__
#define __OWL_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OWL_MAJOR 0
#define OWL_MINOR 1
#define OWL_PATCH 0

#define OWL__STR(x) #x
#define OWL_STR(x) OWL__STR(x)

#define OWL_VERMAJOR OWL_STR(OWL_MAJOR)
#define OWL_VERMINOR OWL_STR(OWL_MINOR)
#define OWL_VERPATCH OWL_STR(OWL_PATCH)

#define OWL_VERNUM ((OWL_MAJOR * 100) + OWL_MINOR)
#define OWL_VERFULL ((OWL_VERNUM * 100) + OWL_PATCH)
#define OWL_VERSION OWL_VERMAJOR "." OWL_VERMINOR
#define OWL_RELEASE OWL_VERSION "." OWL_VERPATCH

#define OWL_AUTHOR "shixiongfei"

#ifndef _WIN32
#define OWL_EXPORT __attribute__((visibility("default")))
#define OWL_IMPORT __attribute__((visibility("default")))
#else
#define OWL_EXPORT __declspec(dllexport)
#define OWL_IMPORT __declspec(dllimport)
#endif

#if defined(OWL_STATIC)
#define OWL_API extern
#elif defined(OWL_BUILD_DLL)
#define OWL_API OWL_EXPORT
#else
#define OWL_API OWL_IMPORT
#endif

#ifdef _MSC_VER
#define OWL_INLINE static __forceinline
#else
#define OWL_INLINE static inline
#endif

#if defined(_WIN32)
#define OWL_WINDOWS 1
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IOS
#define OWL_IOS 1
#elif TARGET_OS_MAC
#define OWL_MACOS 1
#else
#error "Unsupported Platform"
#endif
#define OWL_APPLE 1
#elif defined(__ANDROID__)
#define OWL_ANDROID 1
#elif defined(__linux__)
#define OWL_LINUX 1
#elif defined(__BSD__)
#define OWL_BSD 1
#endif

#define OWL_FORMAT_RGB 3
#define OWL_FORMAT_RGBA 4

#define OWL_BLEND_NONE 0
#define OWL_BLEND_ALPHA 1

#define OWL_AUDIO_U8 0
#define OWL_AUDIO_S8 1
#define OWL_AUDIO_U16 2
#define OWL_AUDIO_S16 3
#define OWL_AUDIO_S32 4
#define OWL_AUDIO_F32 5

#define OWL_EVENT_BASE 0x100
#define OWL_EVENT_QUIT (OWL_EVENT_BASE + 0)
#define OWL_EVENT_KEYDOWN (OWL_EVENT_BASE + 1)
#define OWL_EVENT_KEYUP (OWL_EVENT_BASE + 2)
#define OWL_EVENT_MOUSEMOVE (OWL_EVENT_BASE + 3)
#define OWL_EVENT_MOUSEDOWN (OWL_EVENT_BASE + 4)
#define OWL_EVENT_MOUSEUP (OWL_EVENT_BASE + 5)
#define OWL_EVENT_MOUSEWHEEL (OWL_EVENT_BASE + 6)
#define OWL_EVENT_TEXTINPUT (OWL_EVENT_BASE + 7)
#define OWL_EVENT_TEXTEDITING (OWL_EVENT_BASE + 8)

#define OWL_PI 3.14159265358979323846
#define OWL_DEG (180.0 / OWL_PI)
#define OWL_RAD (OWL_PI / 180.0)

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

typedef union owl_Pixel {
  struct {
    u8 r, g, b, a;
  };
  u32 rgba;
} owl_Pixel;

typedef struct owl_Point {
  s32 x, y;
} owl_Point;

typedef struct owl_Rect {
  s32 x, y;
  s32 w, h;
} owl_Rect;

typedef struct owl_Vector2 {
  f64 x, y;
} owl_Vector2;

typedef struct owl_Matrix {
  f64 a, c, tx;
  f64 b, d, ty;
  /*  0, 0, 1 */
} owl_Matrix;

typedef u32 owl_Audio;
typedef struct SDL_Surface owl_Canvas;

typedef struct owl_Event {
  u32 type;

  union {
    struct {
      s32 code;
    } key;

    struct {
      s32 x, y;
    } mouse;

    struct {
      u8 button;
      s32 x, y;
    } button;

    struct {
      float x, y;
    } wheel;

    struct {
      char *text;
    } input;

    struct {
      char *text;
      s32 start;
      s32 length;
    } edit;
  };
} owl_Event;

typedef enum {
  OWL_KEY_NULL = 0,

  OWL_KEY_LEFTBUTTON = 1,
  OWL_KEY_RIGHTBUTTON = 2,
  OWL_KEY_MIDDLEBUTTON = 3,
  OWL_KEY_X1BUTTON = 4,
  OWL_KEY_X2BUTTON = 5,

  OWL_KEY_BACKSPACE = 8,
  OWL_KEY_TAB = 9,
  OWL_KEY_RETURN = 13,
  OWL_KEY_ESCAPE = 27,
  OWL_KEY_SPACE = 32,
  OWL_KEY_APOSTROPHE = 39,
  OWL_KEY_COMMA = 44,
  OWL_KEY_MINUS = 45,
  OWL_KEY_PERIOD = 46,
  OWL_KEY_SLASH = 47,

  OWL_KEY_0 = 48,
  OWL_KEY_1 = 49,
  OWL_KEY_2 = 50,
  OWL_KEY_3 = 51,
  OWL_KEY_4 = 52,
  OWL_KEY_5 = 53,
  OWL_KEY_6 = 54,
  OWL_KEY_7 = 55,
  OWL_KEY_8 = 56,
  OWL_KEY_9 = 57,

  OWL_KEY_SEMICOLON = 59,
  OWL_KEY_EQUALS = 61,

  OWL_KEY_A = 65,
  OWL_KEY_B = 66,
  OWL_KEY_C = 67,
  OWL_KEY_D = 68,
  OWL_KEY_E = 69,
  OWL_KEY_F = 70,
  OWL_KEY_G = 71,
  OWL_KEY_H = 72,
  OWL_KEY_I = 73,
  OWL_KEY_J = 74,
  OWL_KEY_K = 75,
  OWL_KEY_L = 76,
  OWL_KEY_M = 77,
  OWL_KEY_N = 78,
  OWL_KEY_O = 79,
  OWL_KEY_P = 80,
  OWL_KEY_Q = 81,
  OWL_KEY_R = 82,
  OWL_KEY_S = 83,
  OWL_KEY_T = 84,
  OWL_KEY_U = 85,
  OWL_KEY_V = 86,
  OWL_KEY_W = 87,
  OWL_KEY_X = 88,
  OWL_KEY_Y = 89,
  OWL_KEY_Z = 90,

  OWL_KEY_LEFTBRACKET = 91,
  OWL_KEY_BACKSLASH = 92,
  OWL_KEY_RIGHTBRACKET = 93,
  OWL_KEY_GRAVE = 96,

  OWL_KEY_F1 = 101,
  OWL_KEY_F2 = 102,
  OWL_KEY_F3 = 103,
  OWL_KEY_F4 = 104,
  OWL_KEY_F5 = 105,
  OWL_KEY_F6 = 106,
  OWL_KEY_F7 = 107,
  OWL_KEY_F8 = 108,
  OWL_KEY_F9 = 109,
  OWL_KEY_F10 = 110,
  OWL_KEY_F11 = 111,
  OWL_KEY_F12 = 112,

  OWL_KEY_LEFTCONTROL = 140,
  OWL_KEY_LEFTSHIFT = 141,
  OWL_KEY_LEFTALT = 142,
  OWL_KEY_LEFTSUPER = 143,
  OWL_KEY_RIGHTCONTROL = 144,
  OWL_KEY_RIGHTSHIFT = 145,
  OWL_KEY_RIGHTALT = 146,
  OWL_KEY_RIGHTSUPER = 147,

  OWL_KEY_UP = 160,
  OWL_KEY_DOWN = 161,
  OWL_KEY_LEFT = 162,
  OWL_KEY_RIGHT = 163,

  OWL_KEY_INSERT = 180,
  OWL_KEY_DELETE = 181,
  OWL_KEY_HOME = 182,
  OWL_KEY_END = 183,
  OWL_KEY_PAGEUP = 184,
  OWL_KEY_PAGEDOWN = 185,

  OWL_KEY_CAPSLOCK = 200,
  OWL_KEY_SCROLLLOCK = 201,
  OWL_KEY_NUMLOCK = 202,
  OWL_KEY_PRINTSCREEN = 203,
  OWL_KEY_PAUSE = 204,
  OWL_KEY_APPLICATION = 205,

  OWL_KEY_KP_0 = 220,
  OWL_KEY_KP_1 = 221,
  OWL_KEY_KP_2 = 222,
  OWL_KEY_KP_3 = 223,
  OWL_KEY_KP_4 = 224,
  OWL_KEY_KP_5 = 225,
  OWL_KEY_KP_6 = 226,
  OWL_KEY_KP_7 = 227,
  OWL_KEY_KP_8 = 228,
  OWL_KEY_KP_9 = 229,

  OWL_KEY_KP_PERIOD = 230,
  OWL_KEY_KP_DIVIDE = 231,
  OWL_KEY_KP_MULTIPLY = 232,
  OWL_KEY_KP_MINUS = 233,
  OWL_KEY_KP_PLUS = 234,
  OWL_KEY_KP_ENTER = 235,
  OWL_KEY_KP_EQUALS = 236,

  OWL_KEY_MAX = 256
} owl_Keycode;

OWL_API owl_Pixel owl_rgb(u8 r, u8 g, u8 b);
OWL_API owl_Pixel owl_rgba(u8 r, u8 g, u8 b, u8 a);

OWL_API f64 owl_degrees(f64 rad);
OWL_API f64 owl_radians(f64 deg);

OWL_API void owl_matrix(owl_Matrix *m, f64 a, f64 b, f64 c, f64 d, f64 tx,
                        f64 ty);
OWL_API void owl_matrix_setidentity(owl_Matrix *m);
OWL_API void owl_matrix_settranslate(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_setscale(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_setshear(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_setrotate(owl_Matrix *m, f64 rad);
OWL_API void owl_matrix_settransrotate(owl_Matrix *m, f64 x, f64 y, f64 rad);

OWL_API void owl_matrix_multiply(owl_Matrix *m, owl_Matrix *l, owl_Matrix *r);
OWL_API bool owl_matrix_invert(owl_Matrix *m);

OWL_API void owl_matrix_translate(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_scale(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_shear(owl_Matrix *m, f64 x, f64 y);
OWL_API void owl_matrix_rotate(owl_Matrix *m, f64 rad);
OWL_API void owl_matrix_transrotate(owl_Matrix *m, f64 x, f64 y, f64 rad);
OWL_API void owl_matrix_apply(owl_Matrix *m, owl_Vector2 *out, f64 x, f64 y);

OWL_API const char *owl_version(s32 *major, s32 *minor, s32 *patch);

OWL_API f64 owl_time(u64 *sec, u32 *usec);
OWL_API u64 owl_ticks(void);
OWL_API void owl_sleep(u32 ms);

OWL_API bool owl_init(s32 width, s32 height, const char *title, s32 flags);
OWL_API void owl_quit(void);

OWL_API bool owl_setfps(u32 rate);
OWL_API u32 owl_getfps(void);
OWL_API u32 owl_wait(void);

OWL_API bool owl_event(owl_Event *event);
OWL_API const u8 *owl_keystate(void);
OWL_API void owl_mouse(s32 *x, s32 *y);

OWL_API void owl_textinput(bool onoff);
OWL_API bool owl_textinputactive(void);
OWL_API bool owl_textinputshown(void);
OWL_API void owl_textinputposition(s32 x, s32 y);

OWL_API owl_Canvas *owl_screen(void);
OWL_API owl_Canvas *owl_canvas(s32 width, s32 height);
OWL_API owl_Canvas *owl_image(const u8 *data, s32 w, s32 h, u8 format);
OWL_API owl_Canvas *owl_imagex(const u8 *data, s32 w, s32 h,
                               owl_Pixel colorkey);
OWL_API owl_Canvas *owl_load(const char *filename);
OWL_API owl_Canvas *owl_loadex(const char *filename, owl_Pixel colorkey);
OWL_API void owl_freecanvas(owl_Canvas *canvas);

OWL_API void owl_size(owl_Canvas *canvas, s32 *w, s32 *h);
OWL_API void owl_blendmode(owl_Canvas *canvas, s32 mode);

OWL_API void owl_color(owl_Canvas *canvas, owl_Pixel color);
OWL_API void owl_clear(owl_Canvas *canvas);
OWL_API void owl_fill(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h);

OWL_API void owl_pixel(owl_Canvas *canvas, s32 x, s32 y);
OWL_API void owl_pixels(owl_Canvas *canvas, const owl_Point *points, s32 n);

OWL_API void owl_line(owl_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2);
OWL_API void owl_lines(owl_Canvas *canvas, const owl_Point *points, s32 n);

OWL_API void owl_rect(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h);
OWL_API void owl_fillrect(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h);

OWL_API void owl_ellipse(owl_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry);
OWL_API void owl_fillellipse(owl_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry);

OWL_API void owl_clip(owl_Canvas *canvas, const owl_Rect *rect);
OWL_API void owl_blit(owl_Canvas *canvas, owl_Canvas *src,
                      const owl_Rect *srcrect, const owl_Rect *dstrect);
OWL_API void owl_present(void);

OWL_API bool owl_loadfont(const char *name, const char *filename);
OWL_API bool owl_font(const char *name, s32 size);
OWL_API s32 owl_text(owl_Canvas *canvas, const char *text, s32 x, s32 y,
                     owl_Pixel color);
OWL_API s32 owl_textwidth(const char *text);

OWL_API owl_Audio owl_audio(s32 freq, u8 format, u8 channels, u16 samples);
OWL_API void owl_closeaudio(owl_Audio audio);
OWL_API void owl_clearaudio(owl_Audio audio);
OWL_API void owl_playaudio(owl_Audio audio, bool onoff);
OWL_API bool owl_audiostream(owl_Audio audio, const void *buffer, s32 len);
OWL_API u32 owl_audiobuffered(owl_Audio audio);

OWL_API bool owl_loadsound(const char *name, const char *filename);
OWL_API bool owl_playing(const char *name);
OWL_API bool owl_play(const char *name);
OWL_API bool owl_stop(const char *name);
OWL_API bool owl_pause(const char *name);
OWL_API bool owl_resume(const char *name);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_H__ */
