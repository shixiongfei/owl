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

#define OWL_PATHTYPE_ABSOLUTE 0
#define OWL_PATHTYPE_RELATIVE 1
#define OWL_PATHTYPE_SIMPLE 2

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

#define OWL_MSGBOX_ERROR 0
#define OWL_MSGBOX_WARNING 1
#define OWL_MSGBOX_INFORMATION 2

#define OWL_MSGBOX_NOKEY 0
#define OWL_MSGBOX_RETURNKEY 1
#define OWL_MSGBOX_ESCKEY 2

#define OWL_BUTTON(X) (1 << ((X)-1))
#define OWL_BUTTON_LEFT 1
#define OWL_BUTTON_RIGHT 2
#define OWL_BUTTON_MIDDLE 3
#define OWL_BUTTON_X1 4
#define OWL_BUTTON_X2 5
#define OWL_BUTTON_LMASK OWL_BUTTON(OWL_BUTTON_LEFT)
#define OWL_BUTTON_RMASK OWL_BUTTON(OWL_BUTTON_RIGHT)
#define OWL_BUTTON_MMASK OWL_BUTTON(OWL_BUTTON_MIDDLE)
#define OWL_BUTTON_X1MASK OWL_BUTTON(OWL_BUTTON_X1)
#define OWL_BUTTON_X2MASK OWL_BUTTON(OWL_BUTTON_X2)

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

typedef struct owl_Matrix {
  f32 a, c, tx;
  f32 b, d, ty;
  /*  0, 0, 1 */
} owl_Matrix;

typedef u32 owl_Audio;
typedef struct SDL_Surface owl_Canvas;
typedef struct owl_Table owl_Table;
typedef void (*owl_Dtor)(void *);
typedef void *owl_Dylib;

typedef struct owl_MsgBoxButton {
  u32 flags;
  int buttonid;
  const char *text;
} owl_MsgBoxButton;

typedef struct owl_Event {
  u32 type;

  union {
    struct {
      s32 code;
    } key;

    struct {
      s32 x, y;
      s32 xrel, yrel;
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
  OWL_KEY_UNKNOWN = 0,
  OWL_KEY_A = 4,
  OWL_KEY_B = 5,
  OWL_KEY_C = 6,
  OWL_KEY_D = 7,
  OWL_KEY_E = 8,
  OWL_KEY_F = 9,
  OWL_KEY_G = 10,
  OWL_KEY_H = 11,
  OWL_KEY_I = 12,
  OWL_KEY_J = 13,
  OWL_KEY_K = 14,
  OWL_KEY_L = 15,
  OWL_KEY_M = 16,
  OWL_KEY_N = 17,
  OWL_KEY_O = 18,
  OWL_KEY_P = 19,
  OWL_KEY_Q = 20,
  OWL_KEY_R = 21,
  OWL_KEY_S = 22,
  OWL_KEY_T = 23,
  OWL_KEY_U = 24,
  OWL_KEY_V = 25,
  OWL_KEY_W = 26,
  OWL_KEY_X = 27,
  OWL_KEY_Y = 28,
  OWL_KEY_Z = 29,
  OWL_KEY_1 = 30,
  OWL_KEY_2 = 31,
  OWL_KEY_3 = 32,
  OWL_KEY_4 = 33,
  OWL_KEY_5 = 34,
  OWL_KEY_6 = 35,
  OWL_KEY_7 = 36,
  OWL_KEY_8 = 37,
  OWL_KEY_9 = 38,
  OWL_KEY_0 = 39,
  OWL_KEY_RETURN = 40,
  OWL_KEY_ESCAPE = 41,
  OWL_KEY_BACKSPACE = 42,
  OWL_KEY_TAB = 43,
  OWL_KEY_SPACE = 44,
  OWL_KEY_MINUS = 45,
  OWL_KEY_EQUALS = 46,
  OWL_KEY_LEFTBRACKET = 47,
  OWL_KEY_RIGHTBRACKET = 48,
  OWL_KEY_BACKSLASH = 49,
  OWL_KEY_NONUSHASH = 50,
  OWL_KEY_SEMICOLON = 51,
  OWL_KEY_APOSTROPHE = 52,
  OWL_KEY_GRAVE = 53,
  OWL_KEY_COMMA = 54,
  OWL_KEY_PERIOD = 55,
  OWL_KEY_SLASH = 56,
  OWL_KEY_CAPSLOCK = 57,
  OWL_KEY_F1 = 58,
  OWL_KEY_F2 = 59,
  OWL_KEY_F3 = 60,
  OWL_KEY_F4 = 61,
  OWL_KEY_F5 = 62,
  OWL_KEY_F6 = 63,
  OWL_KEY_F7 = 64,
  OWL_KEY_F8 = 65,
  OWL_KEY_F9 = 66,
  OWL_KEY_F10 = 67,
  OWL_KEY_F11 = 68,
  OWL_KEY_F12 = 69,
  OWL_KEY_PRINTSCREEN = 70,
  OWL_KEY_SCROLLLOCK = 71,
  OWL_KEY_PAUSE = 72,
  OWL_KEY_INSERT = 73,
  OWL_KEY_HOME = 74,
  OWL_KEY_PAGEUP = 75,
  OWL_KEY_DELETE = 76,
  OWL_KEY_END = 77,
  OWL_KEY_PAGEDOWN = 78,
  OWL_KEY_RIGHT = 79,
  OWL_KEY_LEFT = 80,
  OWL_KEY_DOWN = 81,
  OWL_KEY_UP = 82,
  OWL_KEY_NUMLOCKCLEAR = 83,
  OWL_KEY_KP_DIVIDE = 84,
  OWL_KEY_KP_MULTIPLY = 85,
  OWL_KEY_KP_MINUS = 86,
  OWL_KEY_KP_PLUS = 87,
  OWL_KEY_KP_ENTER = 88,
  OWL_KEY_KP_1 = 89,
  OWL_KEY_KP_2 = 90,
  OWL_KEY_KP_3 = 91,
  OWL_KEY_KP_4 = 92,
  OWL_KEY_KP_5 = 93,
  OWL_KEY_KP_6 = 94,
  OWL_KEY_KP_7 = 95,
  OWL_KEY_KP_8 = 96,
  OWL_KEY_KP_9 = 97,
  OWL_KEY_KP_0 = 98,
  OWL_KEY_KP_PERIOD = 99,
  OWL_KEY_NONUSBACKSLASH = 100,
  OWL_KEY_APPLICATION = 101,
  OWL_KEY_POWER = 102,
  OWL_KEY_KP_EQUALS = 103,
  OWL_KEY_F13 = 104,
  OWL_KEY_F14 = 105,
  OWL_KEY_F15 = 106,
  OWL_KEY_F16 = 107,
  OWL_KEY_F17 = 108,
  OWL_KEY_F18 = 109,
  OWL_KEY_F19 = 110,
  OWL_KEY_F20 = 111,
  OWL_KEY_F21 = 112,
  OWL_KEY_F22 = 113,
  OWL_KEY_F23 = 114,
  OWL_KEY_F24 = 115,
  OWL_KEY_EXECUTE = 116,
  OWL_KEY_HELP = 117,
  OWL_KEY_MENU = 118,
  OWL_KEY_SELECT = 119,
  OWL_KEY_STOP = 120,
  OWL_KEY_AGAIN = 121,
  OWL_KEY_UNDO = 122,
  OWL_KEY_CUT = 123,
  OWL_KEY_COPY = 124,
  OWL_KEY_PASTE = 125,
  OWL_KEY_FIND = 126,
  OWL_KEY_MUTE = 127,
  OWL_KEY_VOLUMEUP = 128,
  OWL_KEY_VOLUMEDOWN = 129,
  OWL_KEY_LOCKINGCAPSLOCK = 130,
  OWL_KEY_LOCKINGNUMLOCK = 131,
  OWL_KEY_LOCKINGSCROLLLOCK = 132,
  OWL_KEY_KP_COMMA = 133,
  OWL_KEY_KP_EQUALSAS400 = 134,
  OWL_KEY_INTERNATIONAL1 = 135,
  OWL_KEY_INTERNATIONAL2 = 136,
  OWL_KEY_INTERNATIONAL3 = 137,
  OWL_KEY_INTERNATIONAL4 = 138,
  OWL_KEY_INTERNATIONAL5 = 139,
  OWL_KEY_INTERNATIONAL6 = 140,
  OWL_KEY_INTERNATIONAL7 = 141,
  OWL_KEY_INTERNATIONAL8 = 142,
  OWL_KEY_INTERNATIONAL9 = 143,
  OWL_KEY_LANG1 = 144,
  OWL_KEY_LANG2 = 145,
  OWL_KEY_LANG3 = 146,
  OWL_KEY_LANG4 = 147,
  OWL_KEY_LANG5 = 148,
  OWL_KEY_LANG6 = 149,
  OWL_KEY_LANG7 = 150,
  OWL_KEY_LANG8 = 151,
  OWL_KEY_LANG9 = 152,
  OWL_KEY_ALTERASE = 153,
  OWL_KEY_SYSREQ = 154,
  OWL_KEY_CANCEL = 155,
  OWL_KEY_CLEAR = 156,
  OWL_KEY_PRIOR = 157,
  OWL_KEY_RETURN2 = 158,
  OWL_KEY_SEPARATOR = 159,
  OWL_KEY_OUT = 160,
  OWL_KEY_OPER = 161,
  OWL_KEY_CLEARAGAIN = 162,
  OWL_KEY_CRSEL = 163,
  OWL_KEY_EXSEL = 164,
  OWL_KEY_KP_00 = 176,
  OWL_KEY_KP_000 = 177,
  OWL_KEY_THOUSANDSSEPARATOR = 178,
  OWL_KEY_DECIMALSEPARATOR = 179,
  OWL_KEY_CURRENCYUNIT = 180,
  OWL_KEY_CURRENCYSUBUNIT = 181,
  OWL_KEY_KP_LEFTPAREN = 182,
  OWL_KEY_KP_RIGHTPAREN = 183,
  OWL_KEY_KP_LEFTBRACE = 184,
  OWL_KEY_KP_RIGHTBRACE = 185,
  OWL_KEY_KP_TAB = 186,
  OWL_KEY_KP_BACKSPACE = 187,
  OWL_KEY_KP_A = 188,
  OWL_KEY_KP_B = 189,
  OWL_KEY_KP_C = 190,
  OWL_KEY_KP_D = 191,
  OWL_KEY_KP_E = 192,
  OWL_KEY_KP_F = 193,
  OWL_KEY_KP_XOR = 194,
  OWL_KEY_KP_POWER = 195,
  OWL_KEY_KP_PERCENT = 196,
  OWL_KEY_KP_LESS = 197,
  OWL_KEY_KP_GREATER = 198,
  OWL_KEY_KP_AMPERSAND = 199,
  OWL_KEY_KP_DBLAMPERSAND = 200,
  OWL_KEY_KP_VERTICALBAR = 201,
  OWL_KEY_KP_DBLVERTICALBAR = 202,
  OWL_KEY_KP_COLON = 203,
  OWL_KEY_KP_HASH = 204,
  OWL_KEY_KP_SPACE = 205,
  OWL_KEY_KP_AT = 206,
  OWL_KEY_KP_EXCLAM = 207,
  OWL_KEY_KP_MEMSTORE = 208,
  OWL_KEY_KP_MEMRECALL = 209,
  OWL_KEY_KP_MEMCLEAR = 210,
  OWL_KEY_KP_MEMADD = 211,
  OWL_KEY_KP_MEMSUBTRACT = 212,
  OWL_KEY_KP_MEMMULTIPLY = 213,
  OWL_KEY_KP_MEMDIVIDE = 214,
  OWL_KEY_KP_PLUSMINUS = 215,
  OWL_KEY_KP_CLEAR = 216,
  OWL_KEY_KP_CLEARENTRY = 217,
  OWL_KEY_KP_BINARY = 218,
  OWL_KEY_KP_OCTAL = 219,
  OWL_KEY_KP_DECIMAL = 220,
  OWL_KEY_KP_HEXADECIMAL = 221,
  OWL_KEY_LCTRL = 224,
  OWL_KEY_LSHIFT = 225,
  OWL_KEY_LALT = 226,
  OWL_KEY_LGUI = 227,
  OWL_KEY_RCTRL = 228,
  OWL_KEY_RSHIFT = 229,
  OWL_KEY_RALT = 230,
  OWL_KEY_RGUI = 231,
  OWL_KEY_MODE = 257,
  OWL_KEY_AUDIONEXT = 258,
  OWL_KEY_AUDIOPREV = 259,
  OWL_KEY_AUDIOSTOP = 260,
  OWL_KEY_AUDIOPLAY = 261,
  OWL_KEY_AUDIOMUTE = 262,
  OWL_KEY_MEDIASELECT = 263,
  OWL_KEY_WWW = 264,
  OWL_KEY_MAIL = 265,
  OWL_KEY_CALCULATOR = 266,
  OWL_KEY_COMPUTER = 267,
  OWL_KEY_AC_SEARCH = 268,
  OWL_KEY_AC_HOME = 269,
  OWL_KEY_AC_BACK = 270,
  OWL_KEY_AC_FORWARD = 271,
  OWL_KEY_AC_STOP = 272,
  OWL_KEY_AC_REFRESH = 273,
  OWL_KEY_AC_BOOKMARKS = 274,
  OWL_KEY_BRIGHTNESSDOWN = 275,
  OWL_KEY_BRIGHTNESSUP = 276,
  OWL_KEY_DISPLAYSWITCH = 277,
  OWL_KEY_KBDILLUMTOGGLE = 278,
  OWL_KEY_KBDILLUMDOWN = 279,
  OWL_KEY_KBDILLUMUP = 280,
  OWL_KEY_EJECT = 281,
  OWL_KEY_SLEEP = 282,
  OWL_KEY_APP1 = 283,
  OWL_KEY_APP2 = 284,
  OWL_KEY_AUDIOREWIND = 285,
  OWL_KEY_AUDIOFASTFORWARD = 286,
  OWL_KEY_MAX = 512
} owl_Keycode;

OWL_INLINE owl_Pixel owl_rgb(u8 r, u8 g, u8 b) {
  owl_Pixel p = {r, g, b, 0xFF};
  return p;
}

OWL_INLINE owl_Pixel owl_rgba(u8 r, u8 g, u8 b, u8 a) {
  owl_Pixel p = {r, g, b, a};
  return p;
}

OWL_API u64 owl_ticks(void);
OWL_API void owl_sleep(u32 ms);

OWL_API bool owl_init(s32 width, s32 height, const char *title, s32 flags);
OWL_API void owl_quit(void);

OWL_API bool owl_setfps(u32 rate);
OWL_API u32 owl_getfps(void);
OWL_API u32 owl_wait(void);

OWL_API s32 owl_msgbox(s32 type, const char *title, const char *message,
                       const owl_MsgBoxButton *buttons, s32 count);

OWL_API bool owl_event(owl_Event *event);
OWL_API const u8 *owl_keyboard(void);
OWL_API u32 owl_mouse(s32 *x, s32 *y);

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
OWL_API void owl_destroy(owl_Canvas *canvas);

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

OWL_API owl_Table *owl_table(void);
OWL_API void owl_freetable(owl_Table *table, owl_Dtor dtor);
OWL_API void owl_cleartable(owl_Table *table, owl_Dtor dtor);
OWL_API s32 owl_tablesize(owl_Table *table);
OWL_API void *owl_settable(owl_Table *table, const char *name, void *value);
OWL_API void *owl_gettable(owl_Table *table, const char *name);
OWL_API void *owl_isettable(owl_Table *table, u64 key, void *value);
OWL_API void *owl_igettable(owl_Table *table, u64 key);

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

OWL_API s64 owl_filesize(const char *filename);
OWL_API u8 *owl_readfile(const char *filename);
OWL_API int owl_tempfile(const char *filename);

OWL_API const char *owl_selfname(void);
OWL_API char *owl_pathformat(char *path);
OWL_API char *owl_dirname(char *outbuf, const char *path);
OWL_API char *owl_basename(char *outbuf, const char *path);

OWL_API s32 owl_pathtype(const char *path);
OWL_API char *owl_resolvepath(char *outbuf, const char *path);

OWL_API void owl_setcwd(const char *workdir);
OWL_API char *owl_getcwd(char *workdir, s32 size);

OWL_API bool owl_isexist(const char *path);
OWL_API bool owl_isdir(const char *path);
OWL_API bool owl_isfile(const char *path);
OWL_API bool owl_islink(const char *path);

OWL_API owl_Dylib owl_dylib(const char *sofile);
OWL_API void *owl_dysym(owl_Dylib dylib, const char *name);

#if OWL_WINDOWS
OWL_API u32 owl_pesize(s64 *filesize);
#endif

#ifdef __cplusplus
};
#endif

#endif /* __OWL_H__ */
