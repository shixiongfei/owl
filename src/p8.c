/*
 * p8.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#include "SDL.h"
#include "SDL2_framerate.h"

#include "p8.h"

#define P8_FPS 60

typedef struct p8_Window {
  u8 quit;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
  FPSmanager fps;
} p8_Window;

static int p8_init(p8_Window *app, s32 w, s32 h, const char *title, int flags) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    return -1;

  app->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, w, h, 0);

  if (!app->window)
    return -1;

  app->renderer = SDL_CreateRenderer(
      app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  if (!app->renderer)
    return -1;

  SDL_initFramerate(&app->fps);
  SDL_setFramerate(&app->fps, P8_FPS);

  SDL_DisableScreenSaver();
  app->quit = 0;

  app->screen = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_TARGET, w, h);

  return 0;
}

static void p8_quit(p8_Window *app) {
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

static void p8_input(p8_Window *app) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      app->quit = 1;
      break;
    default:
      break;
    }
  }
}

static void p8_update(p8_Window *app) {
  u32 elapsed;

  SDL_SetRenderTarget(app->renderer, app->screen);

  SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(app->renderer);

  SDL_Rect r = {100, 100, 100, 100};

  SDL_SetRenderDrawColor(app->renderer, 0x00, 0xFF, 0x00, 0x00);
  SDL_RenderFillRect(app->renderer, &r);

  SDL_SetRenderDrawColor(app->renderer, 0xFF, 0x00, 0x00, 0x00);
  SDL_RenderDrawRect(app->renderer, &r);

  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopy(app->renderer, app->screen, NULL, NULL);
  SDL_RenderPresent(app->renderer);

  elapsed = SDL_framerateDelay(&app->fps);
}

int main(int argc, char *argv[]) {
  p8_Window app;

  if (p8_init(&app, 800, 600, "P8-2D", 0) < 0)
    return -1;

  while (!app.quit) {
    p8_input(&app);
    p8_update(&app);
  }

  p8_quit(&app);
  return 0;
}
