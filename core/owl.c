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

#define OWL_WINDOW_FLAGS SDL_WINDOW_ALLOW_HIGHDPI
#define OWL_HW_RENDERER SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
#define OWL_SW_RENDERER SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE

typedef struct owl_Window {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Surface *surface;
  owl_FrameRate fps;
  s32 width, height;
} owl_Window;

static owl_Window owl_app = {0};
static owl_Window *app = &owl_app;

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
  s32 x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  SDL_DisableScreenSaver();

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");

  app->window = SDL_CreateWindow(title, x, y, width, height, OWL_WINDOW_FLAGS);

  if (!app->window)
    goto error;

  app->renderer = SDL_CreateRenderer(app->window, -1, OWL_HW_RENDERER);

  if (!app->renderer)
    app->renderer = SDL_CreateRenderer(app->window, -1, OWL_SW_RENDERER);

  if (!app->renderer)
    goto error;

  app->texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!app->texture)
    goto error;

  app->surface = SDL_CreateRGBSurfaceWithFormatFrom(NULL, width, height, 32, 0,
                                                    SDL_PIXELFORMAT_RGBA32);
  if (!app->surface)
    goto error;

  SDL_LockTexture(app->texture, NULL, &app->surface->pixels,
                  &app->surface->pitch);
  SDL_SetSurfaceRLE(app->surface, SDL_TRUE);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);

  app->width = width;
  app->height = height;

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
  owl_soundquit();
  owl_fontquit();

  if (app->surface) {
    SDL_FreeSurface(app->surface);
    app->surface = NULL;
  }

  if (app->texture) {
    SDL_UnlockTexture(app->texture);
    SDL_DestroyTexture(app->texture);
    app->texture = NULL;
  }

  if (app->renderer) {
    SDL_DestroyRenderer(app->renderer);
    app->renderer = NULL;
  }

  if (app->window) {
    SDL_DestroyWindow(app->window);
    app->window = NULL;
  }

  SDL_Quit();
}

bool owl_setfps(u32 rate) { return owl_framerate_set(&app->fps, rate); }

u32 owl_getfps(void) { return app->fps.rate; }

u32 owl_wait(void) { return owl_framerate_wait(&app->fps); }

owl_Canvas *owl_screen(void) { return app->surface; }

void owl_present(void) {
  SDL_UnlockTexture(app->texture);

  SDL_RenderClear(app->renderer);
  SDL_RenderCopy(app->renderer, app->texture, NULL, NULL);
  SDL_RenderPresent(app->renderer);

  SDL_LockTexture(app->texture, NULL, &app->surface->pixels,
                  &app->surface->pitch);
}
