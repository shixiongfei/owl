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
#include "SDL2_gfxPrimitives.h"

#include "p8.h"

#define P8_PIXELFORMAT SDL_PIXELFORMAT_RGBA8888

typedef struct p8_Window {
  bool quit;
  SDL_Window *window;
  SDL_Renderer *renderer;
  FPSmanager fps;
} p8_Window;

static p8_Window p8_app = {0};

bool p8_init(s32 w, s32 h, const char *title, s32 fps, s32 flags) {
  p8_Window *app = &p8_app;
  s32 x, y, window_flags, renderer_flags;

  x = SDL_WINDOWPOS_CENTERED;
  y = SDL_WINDOWPOS_CENTERED;

  window_flags = SDL_WINDOW_ALLOW_HIGHDPI;
  renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    p8_quit();
    return false;
  }

  app->window = SDL_CreateWindow(title, x, y, w, h, window_flags);

  if (!app->window) {
    p8_quit();
    return false;
  }

  app->renderer = SDL_CreateRenderer(app->window, -1, renderer_flags);

  if (!app->renderer) {
    p8_quit();
    return false;
  }

  SDL_initFramerate(&app->fps);
  SDL_setFramerate(&app->fps, fps);

  SDL_DisableScreenSaver();
  app->quit = 0;

  return true;
}

void p8_quit(void) {
  p8_Window *app = &p8_app;

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

bool p8_closed(void) {
  p8_Window *app = &p8_app;
  return app->quit;
}

void p8_events(void) {
  p8_Window *app = &p8_app;
  SDL_Event event;

  if (!app->window)
    return;

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

void p8_update(p8_Canvas *screen) {
  p8_Window *app = &p8_app;
  u32 elapsed;

  if (!app->renderer)
    return;

  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopy(app->renderer, screen, NULL, NULL);
  SDL_RenderPresent(app->renderer);

  elapsed = SDL_framerateDelay(&app->fps);
}

p8_Canvas *p8_canvas(s32 w, s32 h) {
  p8_Window *app = &p8_app;
  s32 texture_access = SDL_TEXTUREACCESS_TARGET;

  if (!app->renderer)
    return NULL;

  return SDL_CreateTexture(app->renderer, P8_PIXELFORMAT, texture_access, w, h);
}

void p8_destroy(p8_Canvas *canvas) { SDL_DestroyTexture(canvas); }

p8_Pixel p8_rgb(u8 r, u8 g, u8 b) { return p8_rgba(r, g, b, 0xFF); }

p8_Pixel p8_rgba(u8 r, u8 g, u8 b, u8 a) {
  p8_Pixel p = {r, g, b, a};
  return p;
}

static bool p8_target(p8_Window *app, p8_Canvas *canvas) {
  if (!app->renderer)
    return false;

  SDL_SetRenderTarget(app->renderer, canvas);
  return true;
}

void p8_clear(p8_Canvas *canvas, p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas)) {
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(app->renderer);
  }
}

void p8_pixel(p8_Canvas *canvas, s32 x, s32 y, p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    pixelRGBA(app->renderer, x, y, color.r, color.g, color.b, color.a);
}

void p8_line(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
             p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    lineRGBA(app->renderer, x1, y1, x2, y2, color.r, color.g, color.b, color.a);
}

void p8_aaline(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
               p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    aalineRGBA(app->renderer, x1, y1, x2, y2, color.r, color.g, color.b,
               color.a);
}

void p8_thickline(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2, s32 width,
                  p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    thickLineRGBA(app->renderer, x1, y1, x2, y2, width, color.r, color.g,
                  color.b, color.a);
}

void p8_rect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h, p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    rectangleRGBA(app->renderer, x, y, x + w, y + h, color.r, color.g, color.b,
                  color.a);
}

void p8_fillrect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h,
                 p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    boxRGBA(app->renderer, x, y, x + w, y + h, color.r, color.g, color.b,
            color.a);
}

void p8_roundedrect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h, s32 rad,
                    p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    roundedRectangleRGBA(app->renderer, x, y, x + w, y + h, rad, color.r,
                         color.g, color.b, color.a);
}

void p8_fillroundedrect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h, s32 rad,
                        p8_Pixel color) {
  p8_Window *app = &p8_app;

  if (p8_target(app, canvas))
    roundedBoxRGBA(app->renderer, x, y, x + w, y + h, rad, color.r, color.g,
                   color.b, color.a);
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
