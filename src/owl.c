/*
 * owl.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "SDL.h"

#include "owl.h"
#include "owl_font.h"
#include "owl_framerate.h"
#include "owl_sound.h"
#include "owl_window.h"

typedef struct owl_App {
  owl_Window *window;
  owl_FrameRate fps;
} owl_App;

static owl_App owl_app = {0};
static owl_App *app = &owl_app;

const char *owl_version(s32 *major, s32 *minor, s32 *patch) {
  static char owlver[] = {OWL_RELEASE};

  if (major)
    *major = OWL_MAJOR;

  if (minor)
    *minor = OWL_MINOR;

  if (patch)
    *patch = OWL_PATCH;

  return owlver;
}

u64 owl_ticks(void) {
  u64 ticks = SDL_GetTicks64();
  return ticks > 0 ? ticks : 1;
}

void owl_sleep(u32 ms) { SDL_Delay(ms); }

bool owl_init(s32 width, s32 height, const char *title, s32 flags) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  SDL_DisableScreenSaver();

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");

  app->window = owl_window(width, height, title, flags);

  if (!app->window)
    goto error;

  if (!owl_fontinit())
    goto error;

  if (!owl_soundinit())
    goto error;

  owl_setfps(OWL_FRAMERATE_DEFAULT);
  return true;

error:
  owl_quit();
  return false;
}

void owl_quit(void) {
  if (app->window) {
    owl_freewindow(app->window);
    app->window = NULL;
  }

  owl_soundquit();
  owl_fontquit();
  SDL_Quit();
}

bool owl_setfps(u32 rate) { return owl_framerate_set(&app->fps, rate); }

u32 owl_getfps(void) { return app->fps.rate; }

u32 owl_wait(void) { return owl_framerate_wait(&app->fps); }

bool owl_event(owl_Event *event) { return owl_windowevent(app->window, event); }

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

bool owl_textinputshown(void) { return owl_windowtextinputshown(app->window); }

void owl_textinputposition(s32 x, s32 y) {
  owl_windowtextinputposition(app->window, x, y);
}

owl_Canvas *owl_screen(void) { return owl_windowcanvas(app->window); }

void owl_present(void) { owl_windowpresent(app->window); }
