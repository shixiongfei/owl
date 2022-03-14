/*
 * p8_keys.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#ifndef __P8_KEYS_H__
#define __P8_KEYS_H__

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

typedef enum {
  P8K_UNKNOWN = 0,
  P8K_A = 4,
  P8K_B = 5,
  P8K_C = 6,
  P8K_D = 7,
  P8K_E = 8,
  P8K_F = 9,
  P8K_G = 10,
  P8K_H = 11,
  P8K_I = 12,
  P8K_J = 13,
  P8K_K = 14,
  P8K_L = 15,
  P8K_M = 16,
  P8K_N = 17,
  P8K_O = 18,
  P8K_P = 19,
  P8K_Q = 20,
  P8K_R = 21,
  P8K_S = 22,
  P8K_T = 23,
  P8K_U = 24,
  P8K_V = 25,
  P8K_W = 26,
  P8K_X = 27,
  P8K_Y = 28,
  P8K_Z = 29,
  P8K_1 = 30,
  P8K_2 = 31,
  P8K_3 = 32,
  P8K_4 = 33,
  P8K_5 = 34,
  P8K_6 = 35,
  P8K_7 = 36,
  P8K_8 = 37,
  P8K_9 = 38,
  P8K_0 = 39,
  P8K_RETURN = 40,
  P8K_ESCAPE = 41,
  P8K_BACKSPACE = 42,
  P8K_TAB = 43,
  P8K_SPACE = 44,
  P8K_MINUS = 45,
  P8K_EQUALS = 46,
  P8K_LEFTBRACKET = 47,
  P8K_RIGHTBRACKET = 48,
  P8K_BACKSLASH = 49,
  P8K_NONUSHASH = 50,
  P8K_SEMICOLON = 51,
  P8K_APOSTROPHE = 52,
  P8K_GRAVE = 53,
  P8K_COMMA = 54,
  P8K_PERIOD = 55,
  P8K_SLASH = 56,
  P8K_CAPSLOCK = 57,
  P8K_F1 = 58,
  P8K_F2 = 59,
  P8K_F3 = 60,
  P8K_F4 = 61,
  P8K_F5 = 62,
  P8K_F6 = 63,
  P8K_F7 = 64,
  P8K_F8 = 65,
  P8K_F9 = 66,
  P8K_F10 = 67,
  P8K_F11 = 68,
  P8K_F12 = 69,
  P8K_PRINTSCREEN = 70,
  P8K_SCROLLLOCK = 71,
  P8K_PAUSE = 72,
  P8K_INSERT = 73,
  P8K_HOME = 74,
  P8K_PAGEUP = 75,
  P8K_DELETE = 76,
  P8K_END = 77,
  P8K_PAGEDOWN = 78,
  P8K_RIGHT = 79,
  P8K_LEFT = 80,
  P8K_DOWN = 81,
  P8K_UP = 82,
  P8K_NUMLOCKCLEAR = 83,
  P8K_KP_DIVIDE = 84,
  P8K_KP_MULTIPLY = 85,
  P8K_KP_MINUS = 86,
  P8K_KP_PLUS = 87,
  P8K_KP_ENTER = 88,
  P8K_KP_1 = 89,
  P8K_KP_2 = 90,
  P8K_KP_3 = 91,
  P8K_KP_4 = 92,
  P8K_KP_5 = 93,
  P8K_KP_6 = 94,
  P8K_KP_7 = 95,
  P8K_KP_8 = 96,
  P8K_KP_9 = 97,
  P8K_KP_0 = 98,
  P8K_KP_PERIOD = 99,
  P8K_NONUSBACKSLASH = 100,
  P8K_APPLICATION = 101,
  P8K_POWER = 102,
  P8K_KP_EQUALS = 103,
  P8K_F13 = 104,
  P8K_F14 = 105,
  P8K_F15 = 106,
  P8K_F16 = 107,
  P8K_F17 = 108,
  P8K_F18 = 109,
  P8K_F19 = 110,
  P8K_F20 = 111,
  P8K_F21 = 112,
  P8K_F22 = 113,
  P8K_F23 = 114,
  P8K_F24 = 115,
  P8K_EXECUTE = 116,
  P8K_HELP = 117,
  P8K_MENU = 118,
  P8K_SELECT = 119,
  P8K_STOP = 120,
  P8K_AGAIN = 121,
  P8K_UNDO = 122,
  P8K_CUT = 123,
  P8K_COPY = 124,
  P8K_PASTE = 125,
  P8K_FIND = 126,
  P8K_MUTE = 127,
  P8K_VOLUMEUP = 128,
  P8K_VOLUMEDOWN = 129,
  P8K_LOCKINGCAPSLOCK = 130,
  P8K_LOCKINGNUMLOCK = 131,
  P8K_LOCKINGSCROLLLOCK = 132,
  P8K_KP_COMMA = 133,
  P8K_KP_EQUALSAS400 = 134,
  P8K_INTERNATIONAL1 = 135,
  P8K_INTERNATIONAL2 = 136,
  P8K_INTERNATIONAL3 = 137,
  P8K_INTERNATIONAL4 = 138,
  P8K_INTERNATIONAL5 = 139,
  P8K_INTERNATIONAL6 = 140,
  P8K_INTERNATIONAL7 = 141,
  P8K_INTERNATIONAL8 = 142,
  P8K_INTERNATIONAL9 = 143,
  P8K_LANG1 = 144,
  P8K_LANG2 = 145,
  P8K_LANG3 = 146,
  P8K_LANG4 = 147,
  P8K_LANG5 = 148,
  P8K_LANG6 = 149,
  P8K_LANG7 = 150,
  P8K_LANG8 = 151,
  P8K_LANG9 = 152,
  P8K_ALTERASE = 153,
  P8K_SYSREQ = 154,
  P8K_CANCEL = 155,
  P8K_CLEAR = 156,
  P8K_PRIOR = 157,
  P8K_RETURN2 = 158,
  P8K_SEPARATOR = 159,
  P8K_OUT = 160,
  P8K_OPER = 161,
  P8K_CLEARAGAIN = 162,
  P8K_CRSEL = 163,
  P8K_EXSEL = 164,
  P8K_KP_00 = 176,
  P8K_KP_000 = 177,
  P8K_THOUSANDSSEPARATOR = 178,
  P8K_DECIMALSEPARATOR = 179,
  P8K_CURRENCYUNIT = 180,
  P8K_CURRENCYSUBUNIT = 181,
  P8K_KP_LEFTPAREN = 182,
  P8K_KP_RIGHTPAREN = 183,
  P8K_KP_LEFTBRACE = 184,
  P8K_KP_RIGHTBRACE = 185,
  P8K_KP_TAB = 186,
  P8K_KP_BACKSPACE = 187,
  P8K_KP_A = 188,
  P8K_KP_B = 189,
  P8K_KP_C = 190,
  P8K_KP_D = 191,
  P8K_KP_E = 192,
  P8K_KP_F = 193,
  P8K_KP_XOR = 194,
  P8K_KP_POWER = 195,
  P8K_KP_PERCENT = 196,
  P8K_KP_LESS = 197,
  P8K_KP_GREATER = 198,
  P8K_KP_AMPERSAND = 199,
  P8K_KP_DBLAMPERSAND = 200,
  P8K_KP_VERTICALBAR = 201,
  P8K_KP_DBLVERTICALBAR = 202,
  P8K_KP_COLON = 203,
  P8K_KP_HASH = 204,
  P8K_KP_SPACE = 205,
  P8K_KP_AT = 206,
  P8K_KP_EXCLAM = 207,
  P8K_KP_MEMSTORE = 208,
  P8K_KP_MEMRECALL = 209,
  P8K_KP_MEMCLEAR = 210,
  P8K_KP_MEMADD = 211,
  P8K_KP_MEMSUBTRACT = 212,
  P8K_KP_MEMMULTIPLY = 213,
  P8K_KP_MEMDIVIDE = 214,
  P8K_KP_PLUSMINUS = 215,
  P8K_KP_CLEAR = 216,
  P8K_KP_CLEARENTRY = 217,
  P8K_KP_BINARY = 218,
  P8K_KP_OCTAL = 219,
  P8K_KP_DECIMAL = 220,
  P8K_KP_HEXADECIMAL = 221,
  P8K_LCTRL = 224,
  P8K_LSHIFT = 225,
  P8K_LALT = 226,
  P8K_LGUI = 227,
  P8K_RCTRL = 228,
  P8K_RSHIFT = 229,
  P8K_RALT = 230,
  P8K_RGUI = 231,
  P8K_MODE = 257,
  P8K_AUDIONEXT = 258,
  P8K_AUDIOPREV = 259,
  P8K_AUDIOSTOP = 260,
  P8K_AUDIOPLAY = 261,
  P8K_AUDIOMUTE = 262,
  P8K_MEDIASELECT = 263,
  P8K_WWW = 264,
  P8K_MAIL = 265,
  P8K_CALCULATOR = 266,
  P8K_COMPUTER = 267,
  P8K_AC_SEARCH = 268,
  P8K_AC_HOME = 269,
  P8K_AC_BACK = 270,
  P8K_AC_FORWARD = 271,
  P8K_AC_STOP = 272,
  P8K_AC_REFRESH = 273,
  P8K_AC_BOOKMARKS = 274,
  P8K_BRIGHTNESSDOWN = 275,
  P8K_BRIGHTNESSUP = 276,
  P8K_DISPLAYSWITCH = 277,
  P8K_KBDILLUMTOGGLE = 278,
  P8K_KBDILLUMDOWN = 279,
  P8K_KBDILLUMUP = 280,
  P8K_EJECT = 281,
  P8K_SLEEP = 282,
  P8K_APP1 = 283,
  P8K_APP2 = 284,
  P8K_AUDIOREWIND = 285,
  P8K_AUDIOFASTFORWARD = 286,
  P8K_MAX = 512
} P8_Keycode;

#ifdef __cplusplus
};
#endif

#endif /* __P8_KEYS_H__ */
