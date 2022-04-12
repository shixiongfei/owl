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

#include <float.h>
#include <math.h>

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

owl_Pixel owl_rgb(u8 r, u8 g, u8 b) {
  owl_Pixel p = {r, g, b, 0xFF};
  return p;
}

owl_Pixel owl_rgba(u8 r, u8 g, u8 b, u8 a) {
  owl_Pixel p = {r, g, b, a};
  return p;
}

f32 owl_degrees(f32 rad) { return rad * OWL_DEG; }

f32 owl_radians(f32 deg) { return deg * OWL_RAD; }

void owl_matrix(owl_Matrix *m, f32 a, f32 b, f32 c, f32 d, f32 tx, f32 ty) {
  m->a = a, m->c = c, m->tx = tx;
  m->b = b, m->d = d, m->ty = ty;
}

void owl_matrix_setidentity(owl_Matrix *m) {
  m->a = 1.0f, m->c = 0.0f, m->tx = 0.0f;
  m->b = 0.0f, m->d = 1.0f, m->ty = 0.0f;
}

void owl_matrix_settranslate(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, 1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

void owl_matrix_setscale(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, x, 0.0f, 0.0f, y, 0.0f, 0.0f);
}

void owl_matrix_setshear(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, 1.0f, tanf(y), tanf(x), 1.0f, 0.0f, 0.0f);
}

void owl_matrix_setrotate(owl_Matrix *m, f32 rad) {
  f32 c = cosf(rad);
  f32 s = sinf(rad);

  owl_matrix(m, c, s, -s, c, 0.0f, 0.0f);
}

void owl_matrix_settransrotate(owl_Matrix *m, f32 x, f32 y, f32 rad) {
  f32 c = cosf(rad);
  f32 s = sinf(rad);

  f32 cx = x * (1.0f - c) + y * s;
  f32 cy = y * (1.0f - c) - x * s;

  owl_matrix(m, c, s, -s, c, cx, cy);
}

void owl_matrix_multiply(owl_Matrix *m, owl_Matrix *l, owl_Matrix *r) {
  f32 a = l->a * r->a + l->c * r->b;
  f32 b = l->b * r->a + l->d * r->b;
  f32 c = l->a * r->c + l->c * r->d;
  f32 d = l->b * r->c + l->d * r->d;

  f32 tx = l->a * r->tx + l->c * r->ty + l->tx;
  f32 ty = l->b * r->tx + l->d * r->ty + l->ty;

  owl_matrix(m, a, b, c, d, tx, ty);
}

bool owl_matrix_invert(owl_Matrix *m) {
  f32 a, b, c, d, tx, ty, invdet;
  f32 det = m->a * m->d - m->b * m->c;

  if (fabs(det - 0.0f) < FLT_EPSILON)
    return false;

  invdet = 1.0f / det;

  a = invdet * m->d;
  b = -invdet * m->b;
  c = -invdet * m->c;
  d = invdet * m->a;

  tx = (m->c * m->ty - m->d * m->tx) * invdet;
  ty = (m->b * m->tx - m->a * m->ty) * invdet;

  owl_matrix(m, a, b, c, d, tx, ty);
  return true;
}

void owl_matrix_translate(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_settranslate(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_scale(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_setscale(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_shear(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_setshear(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_rotate(owl_Matrix *m, f32 rad) {
  owl_Matrix mat;
  owl_matrix_setrotate(&mat, rad);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_transrotate(owl_Matrix *m, f32 x, f32 y, f32 rad) {
  owl_Matrix mat;
  owl_matrix_settransrotate(&mat, x, y, rad);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_apply(owl_Matrix *m, owl_Vector2 *v, f32 x, f32 y) {
  v->x = (m->a * x) + (m->c * y) + m->tx;
  v->y = (m->b * x) + (m->d * y) + m->ty;
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
