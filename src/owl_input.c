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

static char input_text[32] = {0};
static char edit_text[32] = {0};

OWL_INLINE u8 owl_fixsdlbutton(u8 button) {
  switch (button) {
  case SDL_BUTTON_RIGHT:
    return OWL_BUTTON_RIGHT;
  case SDL_BUTTON_MIDDLE:
    return OWL_BUTTON_MIDDLE;
  }
  return button;
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
      event->key.code = e.key.keysym.scancode;
      return true;

    case SDL_KEYUP:
      event->type = OWL_EVENT_KEYUP;
      event->key.code = e.key.keysym.scancode;
      return true;

    case SDL_MOUSEMOTION:
      event->type = OWL_EVENT_MOUSEMOVE;
      event->mouse.x = e.motion.x;
      event->mouse.y = e.motion.y;
      event->mouse.xrel = e.motion.xrel;
      event->mouse.yrel = e.motion.yrel;
      return true;

    case SDL_MOUSEBUTTONDOWN:
      event->type = OWL_EVENT_MOUSEDOWN;
      event->button.button = owl_fixsdlbutton(e.button.button);
      event->button.x = e.button.x;
      event->button.y = e.button.y;
      return true;

    case SDL_MOUSEBUTTONUP:
      event->type = OWL_EVENT_MOUSEUP;
      event->button.button = owl_fixsdlbutton(e.button.button);
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

const u8 *owl_keyboard(void) { return SDL_GetKeyboardState(NULL); }

u32 owl_mouse(s32 *x, s32 *y) {
  u32 state = SDL_GetMouseState(x, y);
  u32 mask = ~(OWL_BUTTON_RMASK | OWL_BUTTON_MMASK) & state;

  if (SDL_BUTTON_RMASK & state)
    mask |= OWL_BUTTON_RMASK;

  if (SDL_BUTTON_MMASK & state)
    mask |= OWL_BUTTON_MMASK;

  return mask;
}

void owl_textinput(bool onoff) {
  if (onoff)
    SDL_StartTextInput();
  else
    SDL_StopTextInput();
}

bool owl_textinputactive(void) { return SDL_IsTextInputActive(); }

bool owl_textinputshown(void) { return strlen(edit_text) > 0; }

void owl_textinputposition(s32 x, s32 y) {
  SDL_Rect rect;
  s32 w, h;

  owl_size(owl_screen(), &w, &h);

  rect.x = x;
  rect.y = y;
  rect.w = w - x;
  rect.h = h - y;

  SDL_SetTextInputRect(&rect);
}
