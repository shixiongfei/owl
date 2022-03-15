/*
 * p8_defs.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#ifndef __P8_DEFS_H__
#define __P8_DEFS_H__

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

#define P8_FORMAT_RGB 3
#define P8_FORMAT_RGBA 4

#define P8_FLIP_NONE 0
#define P8_FLIP_HORIZONTAL 1
#define P8_FLIP_VERTICAL 2

#define P8_AUDIO_U8 0
#define P8_AUDIO_S8 1
#define P8_AUDIO_U16 2
#define P8_AUDIO_S16 3
#define P8_AUDIO_S32 4
#define P8_AUDIO_F32 5

#define P8_EVENT_BASE 0x100
#define P8_EVENT_QUIT (P8_EVENT_BASE + 0)
#define P8_EVENT_KEYDOWN (P8_EVENT_BASE + 1)
#define P8_EVENT_KEYUP (P8_EVENT_BASE + 2)
#define P8_EVENT_MOUSEMOVE (P8_EVENT_BASE + 3)
#define P8_EVENT_MOUSEDOWN (P8_EVENT_BASE + 4)
#define P8_EVENT_MOUSEUP (P8_EVENT_BASE + 5)
#define P8_EVENT_MOUSEWHEEL (P8_EVENT_BASE + 6)
#define P8_EVENT_TEXTINPUT (P8_EVENT_BASE + 7)
#define P8_EVENT_TEXTEDITING (P8_EVENT_BASE + 8)

#define P8_BUTTON(X) (1 << ((X)-1))
#define P8_BUTTON_LEFT 1
#define P8_BUTTON_MIDDLE 2
#define P8_BUTTON_RIGHT 3
#define P8_BUTTON_X1 4
#define P8_BUTTON_X2 5
#define P8_BUTTON_LMASK P8_BUTTON(P8_BUTTON_LEFT)
#define P8_BUTTON_MMASK P8_BUTTON(P8_BUTTON_MIDDLE)
#define P8_BUTTON_RMASK P8_BUTTON(P8_BUTTON_RIGHT)
#define P8_BUTTON_X1MASK P8_BUTTON(P8_BUTTON_X1)
#define P8_BUTTON_X2MASK P8_BUTTON(P8_BUTTON_X2)

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

typedef u32 p8_Audio;
typedef struct SDL_Texture p8_Canvas;
typedef struct p8_Table p8_Table;
typedef void (*p8_Dtor)(void *);

typedef struct p8_Event {
  u32 type;

  union {
    struct {
      s32 code;
    } key;

    struct {
      u32 state;
      s32 x, y;
      s32 xrel, yrel;
    } mouse;

    struct {
      u8 button;
      u8 clicks;
      s32 x, y;
    } button;

    struct {
      float x, y;
    } wheel;

    struct {
      char text[32];
    } input;

    struct {
      char text[32];
      s32 start;
      s32 length;
    } edit;
  };
} p8_Event;

typedef enum {
  P8_KEY_UNKNOWN = 0,
  P8_KEY_A = 4,
  P8_KEY_B = 5,
  P8_KEY_C = 6,
  P8_KEY_D = 7,
  P8_KEY_E = 8,
  P8_KEY_F = 9,
  P8_KEY_G = 10,
  P8_KEY_H = 11,
  P8_KEY_I = 12,
  P8_KEY_J = 13,
  P8_KEY_K = 14,
  P8_KEY_L = 15,
  P8_KEY_M = 16,
  P8_KEY_N = 17,
  P8_KEY_O = 18,
  P8_KEY_P = 19,
  P8_KEY_Q = 20,
  P8_KEY_R = 21,
  P8_KEY_S = 22,
  P8_KEY_T = 23,
  P8_KEY_U = 24,
  P8_KEY_V = 25,
  P8_KEY_W = 26,
  P8_KEY_X = 27,
  P8_KEY_Y = 28,
  P8_KEY_Z = 29,
  P8_KEY_1 = 30,
  P8_KEY_2 = 31,
  P8_KEY_3 = 32,
  P8_KEY_4 = 33,
  P8_KEY_5 = 34,
  P8_KEY_6 = 35,
  P8_KEY_7 = 36,
  P8_KEY_8 = 37,
  P8_KEY_9 = 38,
  P8_KEY_0 = 39,
  P8_KEY_RETURN = 40,
  P8_KEY_ESCAPE = 41,
  P8_KEY_BACKSPACE = 42,
  P8_KEY_TAB = 43,
  P8_KEY_SPACE = 44,
  P8_KEY_MINUS = 45,
  P8_KEY_EQUALS = 46,
  P8_KEY_LEFTBRACKET = 47,
  P8_KEY_RIGHTBRACKET = 48,
  P8_KEY_BACKSLASH = 49,
  P8_KEY_NONUSHASH = 50,
  P8_KEY_SEMICOLON = 51,
  P8_KEY_APOSTROPHE = 52,
  P8_KEY_GRAVE = 53,
  P8_KEY_COMMA = 54,
  P8_KEY_PERIOD = 55,
  P8_KEY_SLASH = 56,
  P8_KEY_CAPSLOCK = 57,
  P8_KEY_F1 = 58,
  P8_KEY_F2 = 59,
  P8_KEY_F3 = 60,
  P8_KEY_F4 = 61,
  P8_KEY_F5 = 62,
  P8_KEY_F6 = 63,
  P8_KEY_F7 = 64,
  P8_KEY_F8 = 65,
  P8_KEY_F9 = 66,
  P8_KEY_F10 = 67,
  P8_KEY_F11 = 68,
  P8_KEY_F12 = 69,
  P8_KEY_PRINTSCREEN = 70,
  P8_KEY_SCROLLLOCK = 71,
  P8_KEY_PAUSE = 72,
  P8_KEY_INSERT = 73,
  P8_KEY_HOME = 74,
  P8_KEY_PAGEUP = 75,
  P8_KEY_DELETE = 76,
  P8_KEY_END = 77,
  P8_KEY_PAGEDOWN = 78,
  P8_KEY_RIGHT = 79,
  P8_KEY_LEFT = 80,
  P8_KEY_DOWN = 81,
  P8_KEY_UP = 82,
  P8_KEY_NUMLOCKCLEAR = 83,
  P8_KEY_KP_DIVIDE = 84,
  P8_KEY_KP_MULTIPLY = 85,
  P8_KEY_KP_MINUS = 86,
  P8_KEY_KP_PLUS = 87,
  P8_KEY_KP_ENTER = 88,
  P8_KEY_KP_1 = 89,
  P8_KEY_KP_2 = 90,
  P8_KEY_KP_3 = 91,
  P8_KEY_KP_4 = 92,
  P8_KEY_KP_5 = 93,
  P8_KEY_KP_6 = 94,
  P8_KEY_KP_7 = 95,
  P8_KEY_KP_8 = 96,
  P8_KEY_KP_9 = 97,
  P8_KEY_KP_0 = 98,
  P8_KEY_KP_PERIOD = 99,
  P8_KEY_NONUSBACKSLASH = 100,
  P8_KEY_APPLICATION = 101,
  P8_KEY_POWER = 102,
  P8_KEY_KP_EQUALS = 103,
  P8_KEY_F13 = 104,
  P8_KEY_F14 = 105,
  P8_KEY_F15 = 106,
  P8_KEY_F16 = 107,
  P8_KEY_F17 = 108,
  P8_KEY_F18 = 109,
  P8_KEY_F19 = 110,
  P8_KEY_F20 = 111,
  P8_KEY_F21 = 112,
  P8_KEY_F22 = 113,
  P8_KEY_F23 = 114,
  P8_KEY_F24 = 115,
  P8_KEY_EXECUTE = 116,
  P8_KEY_HELP = 117,
  P8_KEY_MENU = 118,
  P8_KEY_SELECT = 119,
  P8_KEY_STOP = 120,
  P8_KEY_AGAIN = 121,
  P8_KEY_UNDO = 122,
  P8_KEY_CUT = 123,
  P8_KEY_COPY = 124,
  P8_KEY_PASTE = 125,
  P8_KEY_FIND = 126,
  P8_KEY_MUTE = 127,
  P8_KEY_VOLUMEUP = 128,
  P8_KEY_VOLUMEDOWN = 129,
  P8_KEY_LOCKINGCAPSLOCK = 130,
  P8_KEY_LOCKINGNUMLOCK = 131,
  P8_KEY_LOCKINGSCROLLLOCK = 132,
  P8_KEY_KP_COMMA = 133,
  P8_KEY_KP_EQUALSAS400 = 134,
  P8_KEY_INTERNATIONAL1 = 135,
  P8_KEY_INTERNATIONAL2 = 136,
  P8_KEY_INTERNATIONAL3 = 137,
  P8_KEY_INTERNATIONAL4 = 138,
  P8_KEY_INTERNATIONAL5 = 139,
  P8_KEY_INTERNATIONAL6 = 140,
  P8_KEY_INTERNATIONAL7 = 141,
  P8_KEY_INTERNATIONAL8 = 142,
  P8_KEY_INTERNATIONAL9 = 143,
  P8_KEY_LANG1 = 144,
  P8_KEY_LANG2 = 145,
  P8_KEY_LANG3 = 146,
  P8_KEY_LANG4 = 147,
  P8_KEY_LANG5 = 148,
  P8_KEY_LANG6 = 149,
  P8_KEY_LANG7 = 150,
  P8_KEY_LANG8 = 151,
  P8_KEY_LANG9 = 152,
  P8_KEY_ALTERASE = 153,
  P8_KEY_SYSREQ = 154,
  P8_KEY_CANCEL = 155,
  P8_KEY_CLEAR = 156,
  P8_KEY_PRIOR = 157,
  P8_KEY_RETURN2 = 158,
  P8_KEY_SEPARATOR = 159,
  P8_KEY_OUT = 160,
  P8_KEY_OPER = 161,
  P8_KEY_CLEARAGAIN = 162,
  P8_KEY_CRSEL = 163,
  P8_KEY_EXSEL = 164,
  P8_KEY_KP_00 = 176,
  P8_KEY_KP_000 = 177,
  P8_KEY_THOUSANDSSEPARATOR = 178,
  P8_KEY_DECIMALSEPARATOR = 179,
  P8_KEY_CURRENCYUNIT = 180,
  P8_KEY_CURRENCYSUBUNIT = 181,
  P8_KEY_KP_LEFTPAREN = 182,
  P8_KEY_KP_RIGHTPAREN = 183,
  P8_KEY_KP_LEFTBRACE = 184,
  P8_KEY_KP_RIGHTBRACE = 185,
  P8_KEY_KP_TAB = 186,
  P8_KEY_KP_BACKSPACE = 187,
  P8_KEY_KP_A = 188,
  P8_KEY_KP_B = 189,
  P8_KEY_KP_C = 190,
  P8_KEY_KP_D = 191,
  P8_KEY_KP_E = 192,
  P8_KEY_KP_F = 193,
  P8_KEY_KP_XOR = 194,
  P8_KEY_KP_POWER = 195,
  P8_KEY_KP_PERCENT = 196,
  P8_KEY_KP_LESS = 197,
  P8_KEY_KP_GREATER = 198,
  P8_KEY_KP_AMPERSAND = 199,
  P8_KEY_KP_DBLAMPERSAND = 200,
  P8_KEY_KP_VERTICALBAR = 201,
  P8_KEY_KP_DBLVERTICALBAR = 202,
  P8_KEY_KP_COLON = 203,
  P8_KEY_KP_HASH = 204,
  P8_KEY_KP_SPACE = 205,
  P8_KEY_KP_AT = 206,
  P8_KEY_KP_EXCLAM = 207,
  P8_KEY_KP_MEMSTORE = 208,
  P8_KEY_KP_MEMRECALL = 209,
  P8_KEY_KP_MEMCLEAR = 210,
  P8_KEY_KP_MEMADD = 211,
  P8_KEY_KP_MEMSUBTRACT = 212,
  P8_KEY_KP_MEMMULTIPLY = 213,
  P8_KEY_KP_MEMDIVIDE = 214,
  P8_KEY_KP_PLUSMINUS = 215,
  P8_KEY_KP_CLEAR = 216,
  P8_KEY_KP_CLEARENTRY = 217,
  P8_KEY_KP_BINARY = 218,
  P8_KEY_KP_OCTAL = 219,
  P8_KEY_KP_DECIMAL = 220,
  P8_KEY_KP_HEXADECIMAL = 221,
  P8_KEY_LCTRL = 224,
  P8_KEY_LSHIFT = 225,
  P8_KEY_LALT = 226,
  P8_KEY_LGUI = 227,
  P8_KEY_RCTRL = 228,
  P8_KEY_RSHIFT = 229,
  P8_KEY_RALT = 230,
  P8_KEY_RGUI = 231,
  P8_KEY_MODE = 257,
  P8_KEY_AUDIONEXT = 258,
  P8_KEY_AUDIOPREV = 259,
  P8_KEY_AUDIOSTOP = 260,
  P8_KEY_AUDIOPLAY = 261,
  P8_KEY_AUDIOMUTE = 262,
  P8_KEY_MEDIASELECT = 263,
  P8_KEY_WWW = 264,
  P8_KEY_MAIL = 265,
  P8_KEY_CALCULATOR = 266,
  P8_KEY_COMPUTER = 267,
  P8_KEY_AC_SEARCH = 268,
  P8_KEY_AC_HOME = 269,
  P8_KEY_AC_BACK = 270,
  P8_KEY_AC_FORWARD = 271,
  P8_KEY_AC_STOP = 272,
  P8_KEY_AC_REFRESH = 273,
  P8_KEY_AC_BOOKMARKS = 274,
  P8_KEY_BRIGHTNESSDOWN = 275,
  P8_KEY_BRIGHTNESSUP = 276,
  P8_KEY_DISPLAYSWITCH = 277,
  P8_KEY_KBDILLUMTOGGLE = 278,
  P8_KEY_KBDILLUMDOWN = 279,
  P8_KEY_KBDILLUMUP = 280,
  P8_KEY_EJECT = 281,
  P8_KEY_SLEEP = 282,
  P8_KEY_APP1 = 283,
  P8_KEY_APP2 = 284,
  P8_KEY_AUDIOREWIND = 285,
  P8_KEY_AUDIOFASTFORWARD = 286,
  P8_KEY_MAX = 512
} P8_Keycode;

#ifdef __cplusplus
};
#endif

#endif /* __P8_DEFS_H__ */
