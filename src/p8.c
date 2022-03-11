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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "p8.h"

#define P8_FPS_UPPER_LIMIT 200
#define P8_FPS_LOWER_LIMIT 1
#define P8_FPS_DEFAULT 60

#define P8_PIXELFORMAT SDL_PIXELFORMAT_RGBA8888
#define P8_BLENDMODE SDL_BLENDMODE_BLEND

typedef struct SDL_Surface p8_Image;
typedef struct stbtt_fontinfo p8_TrueType;

typedef struct p8_Font {
  p8_TrueType *ttf;
  s32 ascent;
  s32 descent;
  s32 linegap;
  f32 scale;
  s32 baseline;
  s32 height;
} p8_Font;

typedef struct p8_Window {
  bool quit;

  SDL_Window *window;
  SDL_Renderer *renderer;

  u32 rate;
  u32 framecount;
  f32 rateticks;
  u64 baseticks;
  u64 lastticks;

  p8_Table *ttfs;
  p8_Font font;
} p8_Window;

static p8_Window p8_app = {0};
static p8_Window *app = &p8_app;

static p8_TrueType *p8_loadttf(const char *filename) {
  u8 *data = p8_readfile(filename);
  p8_TrueType *ttf;
  s32 offset;

  if (!data)
    return NULL;

  ttf = (p8_TrueType *)malloc(sizeof(p8_TrueType));

  if (!ttf) {
    free(data);
    return NULL;
  }

  offset = stbtt_GetFontOffsetForIndex(data, 0);

  if (!stbtt_InitFont(ttf, data, offset)) {
    free(data);
    return NULL;
  }

  return ttf;
}

static void p8_ttffree(p8_TrueType *ttf) {
  free(ttf->data);
  free(ttf);
}

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

  SDL_SetRenderDrawBlendMode(app->renderer, P8_BLENDMODE);
  SDL_DisableScreenSaver();

  app->ttfs = p8_table();

  /* TODO
  if (!app->ttfs) {
    p8_quit();
    return false;
  }
  */

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

  if (app->ttfs) {
    p8_cleartable(app->ttfs, p8_ttffree);
    p8_tablefree(app->ttfs);
    app->ttfs = NULL;
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
  s32 format = P8_PIXELFORMAT;
  s32 access = SDL_TEXTUREACCESS_TARGET;
  p8_Canvas *canvas = SDL_CreateTexture(app->renderer, format, access, w, h);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, P8_BLENDMODE);

  return canvas;
}

static p8_Image *p8_toimage(s32 w, s32 h, const u8 *data, s32 channel) {
  s32 d = channel * 8;
  s32 p = channel * w;
  s32 f = (channel == P8_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                     : SDL_PIXELFORMAT_RGBA32;
  return SDL_CreateRGBSurfaceWithFormatFrom((void *)data, w, h, d, p, f);
}

static p8_Canvas *p8_tocanvas(p8_Image *image) {
  p8_Canvas *canvas = SDL_CreateTextureFromSurface(app->renderer, image);
  SDL_FreeSurface(image);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, P8_BLENDMODE);

  return canvas;
}

p8_Canvas *p8_image(s32 w, s32 h, const u8 *data, s32 format) {
  p8_Image *image = p8_toimage(w, h, data, format);

  if (!image)
    return NULL;

  return p8_tocanvas(image);
}

p8_Canvas *p8_imagex(s32 w, s32 h, const u8 *data, p8_Pixel colorkey) {
  p8_Image *image = p8_toimage(w, h, data, P8_FORMAT_RGB);
  u32 key;

  if (!image)
    return NULL;

  key = SDL_MapRGB(image->format, colorkey.r, colorkey.g, colorkey.b);
  SDL_SetColorKey(image, SDL_TRUE, key);

  return p8_tocanvas(image);
}

p8_Canvas *p8_load(const char *filename) {
  p8_Canvas *canvas;
  s32 w, h, format;
  u8 *data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = p8_image(w, h, data, format);
  stbi_image_free(data);

  return canvas;
}

p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey) {
  p8_Canvas *canvas;
  s32 w, h, format;
  u8 *data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = (format == STBI_rgb) ? p8_imagex(w, h, data, colorkey)
                                : p8_image(w, h, data, format);
  stbi_image_free(data);

  return canvas;
}

void p8_destroy(p8_Canvas *canvas) { SDL_DestroyTexture(canvas); }

void p8_size(p8_Canvas *canvas, s32 *w, s32 *h) {
  SDL_QueryTexture(canvas, NULL, NULL, w, h);
}

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

bool p8_loadfont(const char *name, const char *filename) {
  p8_TrueType *ttf = (p8_TrueType *)p8_gettable(app->ttfs, name);

  ttf = app->font.ttf; // TODO: pending delete

  if (ttf)
    return true;

  ttf = p8_loadttf(filename);

  if (!ttf)
    return false;

  p8_settable(app->ttfs, name, ttf);

  app->font.ttf = ttf; // TODO: pending delete

  return true;
}

bool p8_font(const char *name, s32 size, s32 weight, p8_Pixel color) {
  p8_TrueType *ttf = (p8_TrueType *)p8_gettable(app->ttfs, name);
  s32 ascent, descent, linegap;

  ttf = app->font.ttf; // TODO: pending delete

  if (!ttf)
    return false;

  stbtt_GetFontVMetrics(ttf, &ascent, &descent, &linegap);

  app->font.ttf = ttf;
  app->font.ascent = ascent;
  app->font.descent = descent;
  app->font.linegap = linegap;
  app->font.scale = stbtt_ScaleForMappingEmToPixels(ttf, (f32)size);
  app->font.baseline = (s32)(ascent * app->font.scale);
  app->font.height = (s32)((ascent - descent) * app->font.scale);

  return true;
}

static s32 p8_ttfwide(p8_Font *font, s32 ch, s32 last) {
  s32 ax, lsb, kern = 0;

  stbtt_GetCodepointHMetrics(font->ttf, ch, &ax, &lsb);

  if (last)
    kern = stbtt_GetCodepointKernAdvance(font->ttf, ch, last);

  return (s32)((ax + kern) * font->scale);
}

static u8 *p8_ttfglyph(p8_Font *font, s32 ch, s32 *width, s32 *height) {
  p8_TrueType *ttf = font->ttf;
  f32 scale = font->scale;
  s32 x1, y1, x2, y2;
  s32 x, y, w, h;
  u8 *bitmap, *pixels;

  w = p8_ttfwide(font, ch, 0);
  h = font->height;

  bitmap = (u8 *)malloc(w * h);

  if (!bitmap)
    return NULL;

  memset(bitmap, 0, w * h);

  stbtt_GetCodepointBitmapBoxSubpixel(ttf, ch, scale, scale, 0, 0, &x1, &y1,
                                      &x2, &y2);

  x = x1;
  y = font->baseline + y1;
  pixels = bitmap + x + y * w;

  stbtt_MakeCodepointBitmapSubpixel(ttf, pixels, w - x, h - y, w, scale, scale,
                                    0, 0, ch);

  if (width)
    *width = w;

  if (height)
    *height = h;

  return bitmap;
}

#include <Windows.h>

static void pp(p8_Font *font, s32 ch) {
  s32 i, j, w, h;

  u8 *bitmap = p8_ttfglyph(font, ch, &w, &h);

  // https://github.com/rxi/juno/blob/master/src/ttf.c
  printf("w: %d, h: %d\n", w, h);

  for (j = 0; j < h; ++j) {
    for (i = 0; i < w; ++i) {
      putchar(" .:ioVM@"[bitmap[j * w + i] >> 5]);
    }
    putchar('\n');
  }
  putchar('\n');

  free(bitmap);
}

void p8_text(p8_Canvas *canvas, const p8_Rect *rect, const char *text) {
  p8_Font *font = &app->font;
  

  AllocConsole();
  freopen("CONOUT$", "w", stdout);

  pp(font, L'K');
  pp(font, L'��');
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
