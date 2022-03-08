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

#define P8_FPS 60
#define P8_PIXELFORMAT SDL_PIXELFORMAT_RGBA8888

typedef struct p8_Window {
  bool quit;
  SDL_Window *window;
  SDL_Renderer *renderer;
  FPSmanager fps;
} p8_Window;

static p8_Window p8_app = {0};
static p8_Window *app = &p8_app;

bool p8_init(s16 w, s16 h, const char *title, s32 flags) {
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
  SDL_setFramerate(&app->fps, P8_FPS);

  SDL_DisableScreenSaver();
  app->quit = 0;

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
  if (!app->renderer)
    return;

  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopy(app->renderer, screen, NULL, NULL);
  SDL_RenderPresent(app->renderer);
}

bool p8_setfps(u8 fps) { return 0 == SDL_setFramerate(&app->fps, fps); }

u8 p8_getfps(void) { return (u8)SDL_getFramerate(&app->fps); }

u32 p8_wait(void) { return SDL_framerateDelay(&app->fps); }

p8_Canvas *p8_canvas(s16 w, s16 h) {
  s32 texture_access = SDL_TEXTUREACCESS_TARGET;

  if (!app->renderer)
    return NULL;

  return SDL_CreateTexture(app->renderer, P8_PIXELFORMAT, texture_access, w, h);
}

void p8_destroy(p8_Canvas *canvas) { SDL_DestroyTexture(canvas); }

static bool p8_target(p8_Window *app, p8_Canvas *canvas) {
  if (!app->renderer)
    return false;

  SDL_SetRenderTarget(app->renderer, canvas);
  return true;
}

void p8_clear(p8_Canvas *canvas, p8_Pixel color) {
  if (p8_target(app, canvas)) {
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(app->renderer);
  }
}

void p8_pixel(p8_Canvas *canvas, s16 x, s16 y, p8_Pixel color) {
  if (p8_target(app, canvas))
    pixelRGBA(app->renderer, x, y, color.r, color.g, color.b, color.a);
}

void p8_line(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
             p8_Pixel color) {
  if (p8_target(app, canvas))
    lineRGBA(app->renderer, x1, y1, x2, y2, color.r, color.g, color.b, color.a);
}

void p8_aaline(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2,
               p8_Pixel color) {
  if (p8_target(app, canvas))
    aalineRGBA(app->renderer, x1, y1, x2, y2, color.r, color.g, color.b,
               color.a);
}

void p8_thickline(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2, u8 width,
                  p8_Pixel color) {
  if (p8_target(app, canvas))
    thickLineRGBA(app->renderer, x1, y1, x2, y2, width, color.r, color.g,
                  color.b, color.a);
}

void p8_arc(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start, s16 end,
  p8_Pixel color) {
  if (p8_target(app, canvas))
    arcRGBA(app->renderer, x, y, rad, start, end, color.r, color.g, color.b,
            color.a);
}

void p8_rect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h, p8_Pixel color) {
  if (p8_target(app, canvas))
    rectangleRGBA(app->renderer, x, y, x + w, y + h, color.r, color.g, color.b,
                  color.a);
}

void p8_fillrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h,
                 p8_Pixel color) {
  if (p8_target(app, canvas))
    boxRGBA(app->renderer, x, y, x + w, y + h, color.r, color.g, color.b,
            color.a);
}

void p8_roundedrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h, s16 rad,
                    p8_Pixel color) {
  if (p8_target(app, canvas))
    roundedRectangleRGBA(app->renderer, x, y, x + w, y + h, rad, color.r,
                         color.g, color.b, color.a);
}

void p8_fillroundedrect(p8_Canvas *canvas, s16 x, s16 y, s16 w, s16 h, s16 rad,
                        p8_Pixel color) {
  if (p8_target(app, canvas))
    roundedBoxRGBA(app->renderer, x, y, x + w, y + h, rad, color.r, color.g,
                   color.b, color.a);
}

void p8_circle(p8_Canvas *canvas, s16 x, s16 y, s16 rad, p8_Pixel color) {
  if (p8_target(app, canvas))
    circleRGBA(app->renderer, x, y, rad, color.r, color.g, color.b, color.a);
}

void p8_aacircle(p8_Canvas *canvas, s16 x, s16 y, s16 rad, p8_Pixel color) {
  if (p8_target(app, canvas))
    aacircleRGBA(app->renderer, x, y, rad, color.r, color.g, color.b, color.a);
}

void p8_fillcircle(p8_Canvas *canvas, s16 x, s16 y, s16 rad, p8_Pixel color) {
  if (p8_target(app, canvas))
    filledCircleRGBA(app->renderer, x, y, rad, color.r, color.g, color.b,
                     color.a);
}

void p8_ellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                p8_Pixel color) {
  if (p8_target(app, canvas))
    ellipseRGBA(app->renderer, x, y, rx, ry, color.r, color.g, color.b,
                color.a);
}

void p8_aaellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                  p8_Pixel color) {
  if (p8_target(app, canvas))
    aaellipseRGBA(app->renderer, x, y, rx, ry, color.r, color.g, color.b,
                  color.a);
}

void p8_fillellipse(p8_Canvas *canvas, s16 x, s16 y, s16 rx, s16 ry,
                    p8_Pixel color) {
  if (p8_target(app, canvas))
    filledEllipseRGBA(app->renderer, x, y, rx, ry, color.r, color.g, color.b,
                      color.a);
}

void p8_pie(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start, s16 end,
            p8_Pixel color) {
  if (p8_target(app, canvas))
    pieRGBA(app->renderer, x, y, rad, start, end, color.r, color.g, color.b,
            color.a);
}

void p8_fillpie(p8_Canvas *canvas, s16 x, s16 y, s16 rad, s16 start, s16 end,
                p8_Pixel color) {
  if (p8_target(app, canvas))
    filledPieRGBA(app->renderer, x, y, rad, start, end, color.r, color.g,
                  color.b, color.a);
}

void p8_trigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2, s16 x3,
               s16 y3, p8_Pixel color) {
  if (p8_target(app, canvas))
    trigonRGBA(app->renderer, x1, y1, x2, y2, x3, y3, color.r, color.g, color.b,
               color.a);
}

void p8_aatrigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2, s16 x3,
                 s16 y3, p8_Pixel color) {
  if (p8_target(app, canvas))
    aatrigonRGBA(app->renderer, x1, y1, x2, y2, x3, y3, color.r, color.g,
                 color.b, color.a);
}

void p8_filltrigon(p8_Canvas *canvas, s16 x1, s16 y1, s16 x2, s16 y2, s16 x3,
                   s16 y3, p8_Pixel color) {
  if (p8_target(app, canvas))
    filledTrigonRGBA(app->renderer, x1, y1, x2, y2, x3, y3, color.r, color.g,
                     color.b, color.a);
}

void p8_polygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                p8_Pixel color) {
  if (p8_target(app, canvas))
    polygonRGBA(app->renderer, vx, vy, n, color.r, color.g, color.b, color.a);
}

void p8_aapolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                  p8_Pixel color) {
  if (p8_target(app, canvas))
    aapolygonRGBA(app->renderer, vx, vy, n, color.r, color.g, color.b, color.a);
}

void p8_fillpolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                    p8_Pixel color) {
  if (p8_target(app, canvas))
    filledPolygonRGBA(app->renderer, vx, vy, n, color.r, color.g, color.b,
                      color.a);
}

void p8_bezier(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n, s32 s,
               p8_Pixel color) {
  if (p8_target(app, canvas))
    bezierRGBA(app->renderer, vx, vy, n, s, color.r, color.g, color.b, color.a);
}

void p8_imagepolygon(p8_Canvas *canvas, const s16 *vx, const s16 *vy, s32 n,
                     p8_Image *image, s32 dx, s32 dy) {
  if (p8_target(app, canvas))
    texturedPolygon(app->renderer, vx, vy, n, image, dx, dy);
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
