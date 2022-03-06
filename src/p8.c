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

#include "p8.h"

typedef struct p8_Window {
  SDL_Window *window;
  SDL_Renderer *renderer;
  u8 quit;
} p8_Window;

static int p8_init(p8_Window *app, s32 w, s32 h, const char *title, int flags) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    return -1;

  app->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, w, h, 0);

  if (!app->window)
    return -1;

  app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);

  if (!app->renderer)
    return -1;

  SDL_DisableScreenSaver();
  app->quit = 0;

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
  SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderClear(app->renderer);

  // logic

  SDL_RenderPresent(app->renderer);
  SDL_Delay(16);
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
