/*
 * owl_event.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include <string.h>

#include "SDL.h"

#include "owl_input.h"

static u8 keyboard[OWL_KEY_MAX] = {0};
static char input_text[32] = {0};
static char edit_text[32] = {0};

OWL_INLINE u8 owl_mapSDLButton(u8 button) {
  switch (button) {
  case SDL_BUTTON_RIGHT:
    return OWL_KEY_RIGHTBUTTON;
  case SDL_BUTTON_MIDDLE:
    return OWL_KEY_MIDDLEBUTTON;
  }
  return button;
}

OWL_INLINE s32 owl_mapsSDLKey(s32 scancode) {
  static u8 keymap[SDL_NUM_SCANCODES] = {
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_A,
      OWL_KEY_B,
      OWL_KEY_C,
      OWL_KEY_D,
      OWL_KEY_E,
      OWL_KEY_F,
      OWL_KEY_G,
      OWL_KEY_H,
      OWL_KEY_I,
      OWL_KEY_J,
      OWL_KEY_K,
      OWL_KEY_L,
      OWL_KEY_M,
      OWL_KEY_N,
      OWL_KEY_O,
      OWL_KEY_P,
      OWL_KEY_Q,
      OWL_KEY_R,
      OWL_KEY_S,
      OWL_KEY_T,
      OWL_KEY_U,
      OWL_KEY_V,
      OWL_KEY_W,
      OWL_KEY_X,
      OWL_KEY_Y,
      OWL_KEY_Z,
      OWL_KEY_1,
      OWL_KEY_2,
      OWL_KEY_3,
      OWL_KEY_4,
      OWL_KEY_5,
      OWL_KEY_6,
      OWL_KEY_7,
      OWL_KEY_8,
      OWL_KEY_9,
      OWL_KEY_0,
      OWL_KEY_RETURN,
      OWL_KEY_ESCAPE,
      OWL_KEY_BACKSPACE,
      OWL_KEY_TAB,
      OWL_KEY_SPACE,
      OWL_KEY_MINUS,
      OWL_KEY_EQUALS,
      OWL_KEY_LEFTBRACKET,
      OWL_KEY_RIGHTBRACKET,
      OWL_KEY_BACKSLASH,
      OWL_KEY_NULL,
      OWL_KEY_SEMICOLON,
      OWL_KEY_APOSTROPHE,
      OWL_KEY_GRAVE,
      OWL_KEY_COMMA,
      OWL_KEY_PERIOD,
      OWL_KEY_SLASH,
      OWL_KEY_CAPSLOCK,
      OWL_KEY_F1,
      OWL_KEY_F2,
      OWL_KEY_F3,
      OWL_KEY_F4,
      OWL_KEY_F5,
      OWL_KEY_F6,
      OWL_KEY_F7,
      OWL_KEY_F8,
      OWL_KEY_F9,
      OWL_KEY_F10,
      OWL_KEY_F11,
      OWL_KEY_F12,
      OWL_KEY_PRINTSCREEN,
      OWL_KEY_SCROLLLOCK,
      OWL_KEY_PAUSE,
      OWL_KEY_INSERT,
      OWL_KEY_HOME,
      OWL_KEY_PAGEUP,
      OWL_KEY_DELETE,
      OWL_KEY_END,
      OWL_KEY_PAGEDOWN,
      OWL_KEY_RIGHT,
      OWL_KEY_LEFT,
      OWL_KEY_DOWN,
      OWL_KEY_UP,
      OWL_KEY_NUMLOCK,
      OWL_KEY_KP_DIVIDE,
      OWL_KEY_KP_MULTIPLY,
      OWL_KEY_KP_MINUS,
      OWL_KEY_KP_PLUS,
      OWL_KEY_KP_ENTER,
      OWL_KEY_KP_1,
      OWL_KEY_KP_2,
      OWL_KEY_KP_3,
      OWL_KEY_KP_4,
      OWL_KEY_KP_5,
      OWL_KEY_KP_6,
      OWL_KEY_KP_7,
      OWL_KEY_KP_8,
      OWL_KEY_KP_9,
      OWL_KEY_KP_0,
      OWL_KEY_KP_PERIOD,
      OWL_KEY_NULL,
      OWL_KEY_APPLICATION,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_NULL,
      OWL_KEY_LEFTCONTROL,
      OWL_KEY_LEFTSHIFT,
      OWL_KEY_LEFTALT,
      OWL_KEY_LEFTSUPER,
      OWL_KEY_RIGHTCONTROL,
      OWL_KEY_RIGHTSHIFT,
      OWL_KEY_RIGHTALT,
      OWL_KEY_RIGHTSUPER,
      OWL_KEY_NULL,
  };
  return keymap[scancode];
}

bool owl_event(owl_Event *event) {
  SDL_Event e;

  memset(event, 0, sizeof(owl_Event));

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      event->type = OWL_EVENT_QUIT;
      return true;

    case SDL_KEYDOWN:
      event->type = OWL_EVENT_KEYDOWN;
      event->key.code = owl_mapsSDLKey(e.key.keysym.scancode);
      return true;

    case SDL_KEYUP:
      event->type = OWL_EVENT_KEYUP;
      event->key.code = owl_mapsSDLKey(e.key.keysym.scancode);
      return true;

    case SDL_MOUSEMOTION:
      event->type = OWL_EVENT_MOUSEMOVE;
      event->mouse.x = e.motion.x;
      event->mouse.y = e.motion.y;
      return true;

    case SDL_MOUSEBUTTONDOWN:
      event->type = OWL_EVENT_MOUSEDOWN;
      event->button.button = owl_mapSDLButton(e.button.button);
      event->button.x = e.button.x;
      event->button.y = e.button.y;
      return true;

    case SDL_MOUSEBUTTONUP:
      event->type = OWL_EVENT_MOUSEUP;
      event->button.button = owl_mapSDLButton(e.button.button);
      event->button.x = e.button.x;
      event->button.y = e.button.y;
      return true;

    case SDL_MOUSEWHEEL:
      event->type = OWL_EVENT_MOUSEWHEEL;
      event->wheel.x = e.wheel.preciseX;
      event->wheel.y = e.wheel.preciseY;
      return true;

    case SDL_TEXTINPUT:
      strncpy(input_text, e.text.text, sizeof(input_text));

      event->type = OWL_EVENT_TEXTINPUT;
      event->input.text = input_text;
      return true;

    case SDL_TEXTEDITING:
      strncpy(edit_text, e.edit.text, sizeof(edit_text));

      event->type = OWL_EVENT_TEXTEDITING;
      event->edit.start = e.edit.start;
      event->edit.length = e.edit.length;
      event->edit.text = edit_text;
      return true;
    }
  }
  return false;
}

const u8 *owl_keystate(void) {
  s32 scancode, numkeys = 0;
  u32 mstate = SDL_GetMouseState(NULL, NULL);
  const Uint8 *kstate = SDL_GetKeyboardState(&numkeys);

  memset(keyboard, 0, sizeof(keyboard));

  if (SDL_BUTTON_LMASK & mstate)
    keyboard[OWL_KEY_LEFTBUTTON] = 1;

  if (SDL_BUTTON_RMASK & mstate)
    keyboard[OWL_KEY_RIGHTBUTTON] = 1;

  if (SDL_BUTTON_MMASK & mstate)
    keyboard[OWL_KEY_MIDDLEBUTTON] = 1;

  if (SDL_BUTTON_X1MASK & mstate)
    keyboard[OWL_KEY_X1BUTTON] = 1;

  if (SDL_BUTTON_X2MASK & mstate)
    keyboard[OWL_KEY_X2BUTTON] = 1;

  for (scancode = 0; scancode < numkeys; ++scancode)
    if (kstate[scancode])
      keyboard[owl_mapsSDLKey(scancode)] = 1;

  keyboard[OWL_KEY_NULL] = 0;
  return keyboard;
}

void owl_mouse(s32 *x, s32 *y) { SDL_GetMouseState(x, y); }

void owl_textInput(bool onoff) {
  if (onoff)
    SDL_StartTextInput();
  else
    SDL_StopTextInput();
}

bool owl_textInputActive(void) { return SDL_IsTextInputActive(); }

bool owl_textInputShown(void) { return strlen(edit_text) > 0; }

void owl_textInputPosition(s32 x, s32 y) {
  SDL_Rect rect;
  s32 w, h;

  owl_size(owl_screen(), &w, &h);

  rect.x = x;
  rect.y = y;
  rect.w = w - x;
  rect.h = h - y;

  SDL_SetTextInputRect(&rect);
}
