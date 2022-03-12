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
#include "utf8.h"

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
  SDL_PixelFormat *format;
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

static f32 p8_ttfwide(p8_Font *font, ucs4_t ch, ucs4_t last) {
  s32 ax, lsb, kern = 0;

  stbtt_GetCodepointHMetrics(font->ttf, ch, &ax, &lsb);

  if (last)
    kern = stbtt_GetCodepointKernAdvance(font->ttf, ch, last);

  return (ax + kern) * font->scale;
}

static s32 p8_ttfwidth(p8_Font *font, const char *text) {
  const char *p = text;
  f32 width = 0;
  ucs4_t ch, last = 0;

  while (*p) {
    p += utf8_tounicode(p, &ch);
    width += p8_ttfwide(font, ch, last);
    last = ch;
  }
  return (s32)ceil(width);
}

static u8 *p8_ttfbitmap(p8_Font *font, const char *text, s32 *w, s32 *h) {
  p8_TrueType *ttf = font->ttf;
  const char *p = text;
  ucs4_t ch, last = 0;
  s32 x, y, x1, y1, x2, y2;
  f32 xoff = 0, xfract = 0;
  u8 *bitmap, *pixels;

  if (!ttf)
    return NULL;

  *w = p8_ttfwidth(font, text);
  *h = font->height;

  bitmap = (u8 *)calloc(1, *w * *h);

  if (!bitmap)
    return NULL;

  while (*p) {
    p += utf8_tounicode(p, &ch);

    stbtt_GetCodepointBitmapBoxSubpixel(ttf, ch, font->scale, font->scale,
                                        xfract, 0, &x1, &y1, &x2, &y2);
    x = (s32)(xoff + x1);
    y = font->baseline + y1;

    if (x < 0)
      x = 0;

    if (y < 0)
      y = 0;

    pixels = bitmap + x + (y * *w);

    stbtt_MakeCodepointBitmapSubpixel(ttf, pixels, *w - x, *h - y, *w,
                                      font->scale, font->scale, xfract, 0, ch);
    xoff += p8_ttfwide(font, ch, last);
    xfract = xoff - (int)xoff;
    last = ch;
  }
  return bitmap;
}

static u32 *p8_ttfpixels(p8_Font *font, const char *text, s32 *w, s32 *h) {
  u32 i, size, *pixels;
  u8 *bitmap;

  bitmap = p8_ttfbitmap(font, text, w, h);

  if (!bitmap)
    return NULL;

  size = *w * *h;
  pixels = (u32 *)calloc(1, size * sizeof(Uint32));

  if (!pixels) {
    free(bitmap);
    return NULL;
  }

  for (i = 0; i < size; ++i)
    pixels[i] = SDL_MapRGBA(font->format, 0xFF, 0xFF, 0xFF, bitmap[i]);

  free(bitmap);
  return pixels;
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

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  app->window = SDL_CreateWindow(title, x, y, w, h, window_flags);

  if (!app->window)
    goto error;

  app->renderer = SDL_CreateRenderer(app->window, -1, renderer_flags);

  if (!app->renderer)
    goto error;

  app->font.format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

  if (!app->font.format)
    goto error;

  app->ttfs = p8_table();

  if (!app->ttfs)
    goto error;

  SDL_SetRenderDrawBlendMode(app->renderer, P8_BLENDMODE);
  SDL_DisableScreenSaver();

  app->quit = 0;
  app->framecount = 0;
  app->rate = P8_FPS_DEFAULT;
  app->rateticks = 1000.0f / (f32)P8_FPS_DEFAULT;
  app->baseticks = p8_ticks();
  app->lastticks = app->baseticks;

  return true;

error:
  p8_quit();
  return false;
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
    p8_cleartable(app->ttfs, (p8_Dtor)p8_ttffree);
    p8_tablefree(app->ttfs);
    app->ttfs = NULL;
  }

  if (app->font.format) {
    SDL_FreeFormat(app->font.format);
    app->font.format = NULL;
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

p8_Canvas *p8_image(const u8 *data, s32 w, s32 h, s32 format) {
  p8_Image *image = p8_toimage(w, h, data, format);

  if (!image)
    return NULL;

  return p8_tocanvas(image);
}

p8_Canvas *p8_imagex(const u8 *data, s32 w, s32 h, p8_Pixel colorkey) {
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

  canvas = p8_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey) {
  p8_Canvas *canvas;
  s32 w, h, format;
  u8 *data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = (format == STBI_rgb) ? p8_imagex(data, w, h, colorkey)
                                : p8_image(data, w, h, format);
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

bool p8_font(const char *name, s32 size) {
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
  app->font.height = (s32)ceil((ascent - descent) * app->font.scale);

  return true;
}

void p8_text(p8_Canvas *canvas, const char *text, s32 x, s32 y,
             p8_Pixel color) {
  p8_Font *font = &app->font;
  p8_Rect rect = {x, y, -1, -1};
  p8_Canvas *texture;
  u32 *pixels;

  if (!font)
    return;

  pixels = p8_ttfpixels(font, text, &rect.w, &rect.h);

  if (!pixels)
    return;

  texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32,
                              SDL_TEXTUREACCESS_STATIC, rect.w, rect.h);
  if (!texture) {
    free(pixels);
    return;
  }

  SDL_UpdateTexture(texture, NULL, pixels, rect.w * P8_FORMAT_RGBA);
  free(pixels);

  SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
  SDL_SetTextureAlphaMod(texture, color.a);

  p8_blit(canvas, texture, NULL, &rect);
  p8_destroy(texture);
}

s32 p8_textwidth(const char *text) {
  p8_Font *font = &app->font;

  if (!font)
    return -1;

  return p8_ttfwidth(font, text);
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
