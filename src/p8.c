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

#define P8_FPS_UPPER_LIMIT 200
#define P8_FPS_LOWER_LIMIT 1
#define P8_FPS_DEFAULT 60

#define P8_PIXELFORMAT SDL_PIXELFORMAT_RGBA8888

typedef struct p8_Window {
  bool quit;

  SDL_Window *window;
  SDL_Renderer *renderer;

  u32 rate;
  u32 framecount;
  f32 rateticks;
  u64 baseticks;
  u64 lastticks;
} p8_Window;

static p8_Window p8_app = {0};
static p8_Window *app = &p8_app;

u64 p8_ticks(void) {
  u64 ticks = SDL_GetTicks64();
  return ticks > 0 ? ticks : 1;
}

void p8_sleep(u32 ms) { SDL_Delay(ms); }

bool p8_init(s32 w, s32 h, const char *title, s32 flags) {
  s32 x, y, window_flags, renderer_flags;

  x = SDL_WINDOWPOS_CENTERED;
  y = SDL_WINDOWPOS_CENTERED;

  window_flags = SDL_WINDOW_ALLOW_HIGHDPI;
  renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

  SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
  SDL_DisableScreenSaver();
  app->quit = 0;

  app->framecount = 0;
  app->rate = P8_FPS_DEFAULT;
  app->rateticks = 1000.0f / (f32)P8_FPS_DEFAULT;
  app->baseticks = p8_ticks();
  app->lastticks = app->baseticks;

  return true;
}

void p8_quit(void) {
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

bool p8_closed(void) { return app->quit; }

void p8_events(void) {
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

void p8_present(p8_Canvas *screen) {
  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopy(app->renderer, screen, NULL, NULL);
  SDL_RenderPresent(app->renderer);
}

bool p8_setfps(u32 rate) {
  if (rate < P8_FPS_LOWER_LIMIT || rate > P8_FPS_UPPER_LIMIT)
    return false;

  app->framecount = 0;
  app->rate = rate;
  app->rateticks = 1000.0f / (f32)rate;

  return true;
}

u32 p8_getfps(void) { return app->rate; }

u32 p8_wait(void) {
  u64 current_ticks;
  u64 target_ticks;
  u32 time_passed;

  app->framecount += 1;

  current_ticks = p8_ticks();
  time_passed = (u32)(current_ticks - app->lastticks);

  app->lastticks = current_ticks;

  target_ticks = app->baseticks + (u64)((f32)app->framecount * app->rateticks);

  if (current_ticks <= target_ticks)
    p8_sleep((u32)(target_ticks - current_ticks));
  else {
    app->framecount = 0;
    app->baseticks = p8_ticks();
  }

  return time_passed;
}

p8_Canvas *p8_canvas(s32 w, s32 h) {
  s32 texture_access = SDL_TEXTUREACCESS_TARGET;
  p8_Canvas *canvas= SDL_CreateTexture(app->renderer, P8_PIXELFORMAT, texture_access, w, h);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);

  return canvas;
}

void p8_destroy(p8_Canvas *canvas) { SDL_DestroyTexture(canvas); }

void p8_clear(p8_Canvas *canvas) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderClear(app->renderer);
}

void p8_color(p8_Canvas *canvas, p8_Pixel color) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
}

void p8_pixel(p8_Canvas *canvas, s32 x, s32 y) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawPoint(app->renderer, x, y);
}

void p8_pixels(p8_Canvas *canvas, const p8_Point *points, s32 n) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawPoints(app->renderer, (const SDL_Point *)points, n);
}

void p8_line(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawLine(app->renderer, x1, y1, x2, y2);
}

void p8_lines(p8_Canvas *canvas, const p8_Point *points, s32 n) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawLines(app->renderer, (const SDL_Point *)points, n);
}

void p8_rect(p8_Canvas *canvas, const p8_Rect *rect) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawRect(app->renderer, (const SDL_Rect *)rect);
}

void p8_rects(p8_Canvas *canvas, const p8_Rect *rects, s32 n) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderDrawRects(app->renderer, (const SDL_Rect *)rects, n);
}

void p8_fillrect(p8_Canvas *canvas, const p8_Rect *rect) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderFillRect(app->renderer, (const SDL_Rect *)rect);
}

void p8_fillrects(p8_Canvas *canvas, const p8_Rect *rects, s32 n) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderFillRects(app->renderer, (const SDL_Rect *)rects, n);
}

void p8_clip(p8_Canvas *canvas, const p8_Rect *rect) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderSetClipRect(app->renderer, (const SDL_Rect *)rect);
}

void p8_blit(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
             const p8_Rect *dstrect) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderCopy(app->renderer, src, (const SDL_Rect *)srcrect,
                 (const SDL_Rect *)dstrect);
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
