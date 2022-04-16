/*
 * owl_font.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "SDL.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "utf8.h"

#include "owl_font.h"
#include "owl_canvas.h"
#include "owl_io.h"
#include "owl_table.h"

typedef struct stbtt_fontinfo owl_TrueType;

typedef struct owl_Font {
  owl_TrueType *ttf;
  s32 ascent;
  s32 descent;
  s32 linegap;
  f32 scale;
  f32 baseline;
  f32 height;
} owl_Font;

static owl_Table *ttfs = NULL;
static owl_Font font = {0};

static owl_TrueType *owl_loadttf(const char *filename) {
  u8 *data = owl_readfile(filename);
  owl_TrueType *ttf;
  s32 offset;

  if (!data)
    return NULL;

  ttf = (owl_TrueType *)malloc(sizeof(owl_TrueType));

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

static void owl_freettf(owl_TrueType *ttf) {
  free(ttf->data);
  free(ttf);
}

static f32 owl_fontwide(owl_Font *font, ucs4_t ch, ucs4_t last) {
  s32 ax, lsb, kern = 0;

  stbtt_GetCodepointHMetrics(font->ttf, ch, &ax, &lsb);

  if (last)
    kern = stbtt_GetCodepointKernAdvance(font->ttf, ch, last);

  return (ax + kern) * font->scale;
}

static f32 owl_fontwidth(owl_Font *font, const char *text) {
  const char *p = text;
  f32 width = 0;
  ucs4_t ch, last = 0;

  while (*p) {
    p += utf8_tounicode(p, &ch);
    width += owl_fontwide(font, ch, last);
    last = ch;
  }
  return ceilf(width);
}

static u8 *owl_fontbitmap(owl_Font *font, const char *text, s32 *w, s32 *h) {
  owl_TrueType *ttf = font->ttf;
  const char *p = text;
  ucs4_t ch, last = 0;
  s32 x, y, x1, y1, x2, y2;
  f32 xoff = 0, xfract = 0;
  u8 *bitmap, *pixels;

  if (!ttf)
    return NULL;

  *w = (s32)owl_fontwidth(font, text);
  *h = (s32)font->height;

  bitmap = (u8 *)calloc(1, *w * *h);

  if (!bitmap)
    return NULL;

  while (*p) {
    p += utf8_tounicode(p, &ch);

    stbtt_GetCodepointBitmapBoxSubpixel(ttf, ch, font->scale, font->scale,
                                        xfract, 0, &x1, &y1, &x2, &y2);
    x = (s32)(xoff + x1);
    y = (s32)(font->baseline + y1);

    if (x < 0)
      x = 0;

    if (y < 0)
      y = 0;

    pixels = bitmap + x + (y * *w);

    stbtt_MakeCodepointBitmapSubpixel(ttf, pixels, *w - x, *h - y, *w,
                                      font->scale, font->scale, xfract, 0, ch);
    xoff += owl_fontwide(font, ch, last);
    xfract = xoff - (int)xoff;
    last = ch;
  }
  return bitmap;
}

bool owl_fontinit(void) {
  if (!ttfs)
    ttfs = owl_table();
  return ttfs != NULL;
}

void owl_fontquit(void) {
  if (ttfs) {
    owl_freetable(ttfs, (owl_Dtor)owl_freettf);
    ttfs = NULL;
  }
  memset(&font, 0, sizeof(owl_Font));
}

bool owl_loadfont(const char *name, const char *filename) {
  owl_TrueType *ttf = (owl_TrueType *)owl_gettable(ttfs, name);

  if (ttf)
    return true;

  if (!filename)
    return false;

  ttf = owl_loadttf(filename);

  if (!ttf)
    return false;

  owl_settable(ttfs, name, ttf);
  return true;
}

bool owl_font(const char *name, s32 size) {
  owl_TrueType *ttf = (owl_TrueType *)owl_gettable(ttfs, name);
  s32 ascent, descent, linegap;

  if (!ttf)
    return false;

  stbtt_GetFontVMetrics(ttf, &ascent, &descent, &linegap);

  font.ttf = ttf;
  font.ascent = ascent;
  font.descent = descent;
  font.linegap = linegap;
  font.scale = stbtt_ScaleForMappingEmToPixels(ttf, (f32)size);
  font.baseline = ceilf(ascent * font.scale);
  font.height = ceilf((ascent - descent) * font.scale);

  return true;
}

s32 owl_text(owl_Canvas *canvas, const char *text, s32 x, s32 y,
             owl_Pixel color) {
  owl_Rect rect = {x, y, -1, -1};
  f32 alpha = (f32)color.a / 0xFF;
  u8 *bitmap;
  s32 i, j;

  if (!text)
    return -1;

  bitmap = owl_fontbitmap(&font, text, &rect.w, &rect.h);

  if (!bitmap)
    return -1;

  owl_lockcanvas(canvas);
  for (j = 0; j < rect.h; ++j)
    for (i = 0; i < rect.w; ++i) {
      u32 pixel = SDL_MapRGBA(canvas->format, color.r, color.g, color.b,
                              (u8)ceilf(alpha * bitmap[j * rect.w + i]));
      owl_drawpixel(canvas, x + i, y + j, pixel);
    }
  owl_unlockcanvas(canvas);
  free(bitmap);

  return rect.w;
}

s32 owl_textwidth(const char *text) {
  if (!text)
    return -1;

  return (s32)owl_fontwidth(&font, text);
}
