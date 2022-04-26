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
  owl_Canvas *target;
  owl_FrameRate fps;
  s32 width, height;
} owl_Window;

static owl_Window owl_app = {0};
static owl_Window *app = &owl_app;

owl_Pixel owl_rgb(u8 r, u8 g, u8 b) {
  owl_Pixel p = {r, g, b, 0xFF};
  return p;
}

owl_Pixel owl_rgba(u8 r, u8 g, u8 b, u8 a) {
  owl_Pixel p = {r, g, b, a};
  return p;
}

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

static SDL_Surface *owl_surface(const u8 *data, s32 w, s32 h, u8 format) {
  s32 d = format * 8;
  s32 p = format * w;
  s32 f = (format == OWL_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                     : SDL_PIXELFORMAT_RGBA32;
  SDL_Surface *surface;

  if (!data)
    return NULL;

  surface = SDL_CreateRGBSurfaceWithFormatFrom((void *)data, w, h, d, p, f);

  if (!surface)
    return NULL;

  if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
    SDL_Surface *origin = surface;

    surface = SDL_ConvertSurfaceFormat(origin, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(origin);
  }
  return surface;
}

static owl_Canvas *owl_surface_totexture(SDL_Surface *surface) {
  owl_Canvas *canvas = SDL_CreateTextureFromSurface(app->renderer, surface);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
  return canvas;
}

owl_Canvas *owl_texture(s32 access, s32 w, s32 h) {
  return SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, access, w, h);
}

bool owl_init(s32 width, s32 height, const char *title, s32 flags) {
  s32 x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  SDL_DisableScreenSaver();

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
  SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "3");
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

  app->texture = owl_texture(SDL_TEXTUREACCESS_TARGET, width, height);

  if (!app->texture)
    goto error;

  SDL_SetTextureBlendMode(app->texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(app->renderer, app->texture);

  SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);

  app->target = app->texture;
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

  if (app->texture) {
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

owl_Canvas *owl_screen(void) { return app->texture; }

owl_Canvas *owl_canvas(s32 width, s32 height) {
  owl_Canvas *canvas = owl_texture(SDL_TEXTUREACCESS_TARGET, width, height);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
  return canvas;
}

owl_Canvas *owl_image(const u8 *data, s32 w, s32 h, u8 format) {
  SDL_Surface *surface = owl_surface(data, w, h, format);

  if (!surface)
    return NULL;

  return owl_surface_totexture(surface);
}

owl_Canvas *owl_imagex(const u8 *data, s32 w, s32 h, owl_Pixel colorkey) {
  SDL_Surface *surface = owl_surface(data, w, h, OWL_FORMAT_RGB);
  u32 key;

  if (!surface)
    return NULL;

  key = SDL_MapRGB(surface->format, colorkey.r, colorkey.g, colorkey.b);
  SDL_SetColorKey(surface, SDL_TRUE, key);

  return owl_surface_totexture(surface);
}

void owl_freecanvas(owl_Canvas *canvas) {
  if (canvas == app->texture)
    return;

  if (canvas == app->target)
    owl_target(app->texture);

  if (canvas)
    SDL_DestroyTexture(canvas);
}

void owl_size(owl_Canvas *canvas, s32 *w, s32 *h) {
  SDL_QueryTexture(canvas, NULL, NULL, w, h);
}

static void owl_setblendmode(owl_Canvas *canvas) {
  SDL_BlendMode blendmode;

  if (0 == SDL_GetTextureBlendMode(canvas, &blendmode))
    SDL_SetRenderDrawBlendMode(app->renderer, blendmode);
}

void owl_blendmode(owl_Canvas *canvas, s32 mode) {
  switch (mode) {
  case OWL_BLEND_ALPHA:
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
    break;
  case OWL_BLEND_NONE:
  default:
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_NONE);
    break;
  }

  if (app->target == canvas)
    owl_setblendmode(canvas);
}

void owl_target(owl_Canvas *canvas) {
  if (!canvas)
    canvas = app->texture;

  if (app->target != canvas) {
    app->target = canvas;
    SDL_SetRenderTarget(app->renderer, canvas);
    owl_setblendmode(canvas);
  }
}

void owl_color(owl_Pixel color) {
  SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
}

void owl_clear(void) { SDL_RenderClear(app->renderer); }

void owl_pixel(f32 x, f32 y) { SDL_RenderDrawPointF(app->renderer, x, y); }

void owl_pixels(const owl_Point *points, s32 n) {
  SDL_RenderDrawPointsF(app->renderer, (const SDL_FPoint *)points, n);
}

void owl_line(f32 x1, f32 y1, f32 x2, f32 y2) {
  SDL_RenderDrawLineF(app->renderer, x1, y1, x2, y2);
}

void owl_lines(const owl_Point *points, s32 n) {
  SDL_RenderDrawLinesF(app->renderer, (const SDL_FPoint *)points, n);
}

void owl_rect(f32 x, f32 y, f32 w, f32 h) {
  SDL_FRect rect = {x, y, w, h};
  SDL_RenderDrawRectF(app->renderer, &rect);
}

void owl_rects(const owl_Rect *rects, s32 n) {
  SDL_RenderDrawRectsF(app->renderer, (const SDL_FRect *)rects, n);
}

void owl_fillrect(f32 x, f32 y, f32 w, f32 h) {
  SDL_FRect rect = {x, y, w, h};
  SDL_RenderFillRectF(app->renderer, &rect);
}

void owl_fillrects(const owl_Rect *rects, s32 n) {
  SDL_RenderFillRectsF(app->renderer, (const SDL_FRect *)rects, n);
}

void owl_geometry(owl_Canvas *texture, const owl_Vertex *vertices,
                  s32 num_vertices, const s32 *indices, s32 num_indices) {
  const f32 *xy = (const f32 *)&vertices->position;
  const SDL_Color *color = (const SDL_Color *)&vertices->color;
  const f32 *uv = (const f32 *)&vertices->uv;

  SDL_RenderGeometryRaw(app->renderer, texture, xy, (s32)sizeof(owl_Vertex),
                        color, (s32)sizeof(owl_Vertex), uv,
                        (s32)sizeof(owl_Vertex), num_vertices, indices,
                        num_indices, sizeof(s32));
}

void owl_clip(const owl_Rect *rect) {
  SDL_Rect irect, *clip = NULL;

  if (rect) {
    irect.x = (s32)ceilf(rect->x);
    irect.y = (s32)ceilf(rect->y);
    irect.w = (s32)ceilf(rect->w);
    irect.h = (s32)ceilf(rect->h);
    clip = &irect;
  }

  SDL_RenderSetClipRect(app->renderer, clip);
}

void owl_blit(owl_Canvas *canvas, const owl_Rect *srcrect,
              const owl_Rect *dstrect, f64 angle, const owl_Point *center,
              u8 flip) {
  SDL_Rect rect, *src = NULL;

  if (srcrect) {
    rect.x = (s32)ceilf(srcrect->x);
    rect.y = (s32)ceilf(srcrect->y);
    rect.w = (s32)ceilf(srcrect->w);
    rect.h = (s32)ceilf(srcrect->h);
    src = &rect;
  }

  SDL_RenderCopyExF(app->renderer, canvas, src, (const SDL_FRect *)dstrect,
                    angle, (const SDL_FPoint *)center, flip);
}

void owl_present(void) {
  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopyF(app->renderer, app->texture, NULL, NULL);
  SDL_RenderPresent(app->renderer);

  app->target = NULL;
  owl_target(app->texture);
}
