/*
 * owl_window.c
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
#include "owl_window.h"

#define OWL_WINDOW_FLAGS SDL_WINDOW_ALLOW_HIGHDPI
#define OWL_HW_RENDERER SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
#define OWL_SW_RENDERER SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE

struct owl_Window {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Surface *surface;

  s32 width, height;
  char input_text[32];
  char edit_text[32];
};

owl_Window *owl_window(s32 w, s32 h, const char *title, s32 flags) {
  s32 x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;
  owl_Window *window = (owl_Window *)malloc(sizeof(owl_Window));

  if (!window)
    return NULL;

  memset(window, 0, sizeof(owl_Window));

  window->window = SDL_CreateWindow(title, x, y, w, h, OWL_WINDOW_FLAGS);

  if (!window->window)
    goto error;

  window->renderer = SDL_CreateRenderer(window->window, -1, OWL_HW_RENDERER);

  if (!window->renderer)
    window->renderer = SDL_CreateRenderer(window->window, -1, OWL_SW_RENDERER);

  if (!window->renderer)
    goto error;

  window->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA32,
                                      SDL_TEXTUREACCESS_STREAMING, w, h);
  if (!window->texture)
    goto error;

  window->surface = SDL_CreateRGBSurfaceWithFormatFrom(NULL, w, h, 32, 0,
                                                       SDL_PIXELFORMAT_RGBA32);
  if (!window->surface)
    goto error;

  SDL_LockTexture(window->texture, NULL, &window->surface->pixels,
                  &window->surface->pitch);
  SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 0);

  window->width = w;
  window->height = h;

  return window;

error:
  owl_freewindow(window);
  return NULL;
}

void owl_freewindow(owl_Window *window) {
  if (window->surface) {
    SDL_FreeSurface(window->surface);
    window->surface = NULL;
  }

  if (window->texture) {
    SDL_UnlockTexture(window->texture);
    SDL_DestroyTexture(window->texture);
    window->texture = NULL;
  }

  if (window->renderer) {
    SDL_DestroyRenderer(window->renderer);
    window->renderer = NULL;
  }

  if (window->window) {
    SDL_DestroyWindow(window->window);
    window->window = NULL;
  }

  free(window);
}

OWL_INLINE u8 owl_fixsdlbutton(u8 button) {
  switch (button) {
  case SDL_BUTTON_RIGHT:
    return OWL_BUTTON_RIGHT;
  case SDL_BUTTON_MIDDLE:
    return OWL_BUTTON_MIDDLE;
  }
  return button;
}

bool owl_windowevent(owl_Window *window, owl_Event *event) {
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
      strncpy(window->input_text, e.text.text, sizeof(window->input_text));

      event->type = OWL_EVENT_TEXTINPUT;
      event->input.text = window->input_text;
      return true;

    case SDL_TEXTEDITING:
      strncpy(window->edit_text, e.edit.text, sizeof(window->edit_text));

      event->type = OWL_EVENT_TEXTEDITING;
      event->edit.start = e.edit.start;
      event->edit.length = e.edit.length;
      event->edit.text = window->edit_text;
      return true;
    }
  }
  return false;
}

bool owl_windowtextinputshown(owl_Window *window) {
  return strlen(window->edit_text) > 0;
}

void owl_windowtextinputposition(owl_Window *window, s32 x, s32 y) {
  SDL_Rect rect = {x, y, window->width - x, window->height - y};
  SDL_SetTextInputRect(&rect);
}

owl_Canvas *owl_windowcanvas(owl_Window *window) { return window->surface; }

void owl_windowpresent(owl_Window *window) {
  SDL_UnlockTexture(window->texture);

  SDL_RenderClear(window->renderer);
  SDL_RenderCopy(window->renderer, window->texture, NULL, NULL);
  SDL_RenderPresent(window->renderer);

  SDL_LockTexture(window->texture, NULL, &window->surface->pixels,
                  &window->surface->pitch);
}
