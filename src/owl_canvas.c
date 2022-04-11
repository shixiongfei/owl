/*
 * owl_canvas.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "SDL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "owl_canvas.h"

static owl_Canvas *owl_surface(const u8 *data, s32 w, s32 h, u8 format) {
  s32 d = format * 8;
  s32 p = format * w;
  s32 f = (format == OWL_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                     : SDL_PIXELFORMAT_RGBA32;
  if (!data)
    return NULL;

  return SDL_CreateRGBSurfaceWithFormatFrom((void *)data, w, h, d, p, f);
}

static owl_Canvas *owl_tocanvas(owl_Canvas *surface) {
  owl_Canvas *canvas;

  if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
    canvas = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    SDL_SetSurfaceBlendMode(canvas, SDL_BLENDMODE_BLEND);
    return canvas;
  }

  SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
  return surface;
}

OWL_INLINE u32 owl_getcolor(owl_Canvas *canvas) {
  return (u32)(uword_t)canvas->userdata;
}

OWL_INLINE u32 *owl_getpixel(owl_Canvas *canvas, s32 x, s32 y) {
  return (u32 *)((u8 *)canvas->pixels + y * canvas->pitch +
                 x * canvas->format->BytesPerPixel);
}

OWL_INLINE u8 owl_blend(u32 d, u32 s, u8 a) {
  return (s + ((d - s) * a >> 8)) & 0xFF;
}

OWL_INLINE bool owl_clipped(owl_Canvas *canvas, s32 x, s32 y) {
  SDL_Point point = {x, y};
  SDL_Rect clip;

  SDL_GetClipRect(canvas, &clip);
  return !SDL_PointInRect(&point, &clip);
}

void owl_lock(owl_Canvas *canvas) {
  if (SDL_MUSTLOCK(canvas))
    SDL_LockSurface(canvas);
}

void owl_unlock(owl_Canvas *canvas) {
  if (SDL_MUSTLOCK(canvas))
    SDL_UnlockSurface(canvas);
}

void owl_drawpixel(owl_Canvas *canvas, s32 x, s32 y, u32 color) {
  u8 r, g, b, a;
  u8 R, G, B, A;
  u8 dR, dG, dB, dA;
  SDL_BlendMode mode;
  u32 *pixel;

  if (owl_clipped(canvas, x, y))
    return;

  pixel = owl_getpixel(canvas, x, y);

  if (!pixel)
    return;

  SDL_GetSurfaceBlendMode(canvas, &mode);

  if (mode != SDL_BLENDMODE_BLEND) {
    *pixel = color;
    return;
  }

  SDL_GetRGBA(color, canvas->format, &r, &g, &b, &a);

  if (a == 0)
    return;

  if (a == 0xFF) {
    *pixel = color;
    return;
  }

  SDL_GetRGBA(*pixel, canvas->format, &R, &G, &B, &A);

  dR = owl_blend(r, R, a);
  dG = owl_blend(g, G, a);
  dB = owl_blend(b, B, a);
  dA = owl_blend(a, A, a);

  *pixel = SDL_MapRGBA(canvas->format, dR, dG, dB, dA);
}

void owl_drawline(owl_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
                  u32 color) {
  s32 dx = abs(x2 - x1);
  s32 dy = abs(y2 - y1);
  s32 sx = x1 < x2 ? 1 : -1;
  s32 sy = y1 < y2 ? 1 : -1;
  s32 err = dx - dy;
  s32 e2;

  do {
    owl_drawpixel(canvas, x1, y1, color);

    e2 = 2 * err;

    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }

    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  } while (x1 != x2 || y1 != y2);
}

owl_Canvas *owl_canvas(s32 width, s32 height) {
  owl_Canvas *canvas = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32,
                                                      SDL_PIXELFORMAT_RGBA32);
  if (!canvas)
    return NULL;

  SDL_SetSurfaceBlendMode(canvas, SDL_BLENDMODE_BLEND);
  return canvas;
}

owl_Canvas *owl_image(const u8 *data, s32 w, s32 h, u8 format) {
  owl_Canvas *surface = owl_surface(data, w, h, format);

  if (!surface)
    return NULL;

  return owl_tocanvas(surface);
}

owl_Canvas *owl_imagex(const u8 *data, s32 w, s32 h, owl_Pixel colorkey) {
  owl_Canvas *surface = owl_surface(data, w, h, OWL_FORMAT_RGB);
  u32 key;

  if (!surface)
    return NULL;

  key = SDL_MapRGB(surface->format, colorkey.r, colorkey.g, colorkey.b);
  SDL_SetColorKey(surface, SDL_TRUE, key);

  return owl_tocanvas(surface);
}

owl_Canvas *owl_load(const char *filename) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

owl_Canvas *owl_loadex(const char *filename, owl_Pixel colorkey) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = (format == STBI_rgb) ? owl_imagex(data, w, h, colorkey)
                                : owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

void owl_destroy(owl_Canvas *canvas) {
  if (canvas)
    SDL_FreeSurface(canvas);
}

void owl_size(owl_Canvas *canvas, s32 *w, s32 *h) {
  if (w)
    *w = canvas->w;

  if (h)
    *h = canvas->h;
}

void owl_blendmode(owl_Canvas *canvas, s32 mode) {
  switch (mode) {
  case OWL_BLEND_ALPHA:
    SDL_SetSurfaceBlendMode(canvas, SDL_BLENDMODE_BLEND);
    break;
  case OWL_BLEND_NONE:
  default:
    SDL_SetSurfaceBlendMode(canvas, SDL_BLENDMODE_NONE);
    break;
  }
}

void owl_pixel(owl_Canvas *canvas, s32 x, s32 y) {
  owl_Point point = {x, y};
  owl_pixels(canvas, &point, 1);
}

void owl_pixels(owl_Canvas *canvas, const owl_Point *points, s32 n) {
  u32 color = owl_getcolor(canvas);

  owl_lock(canvas);
  while (n-- > 0)
    owl_drawpixel(canvas, points[n].x, points[n].y, color);
  owl_unlock(canvas);
}

void owl_color(owl_Canvas *canvas, owl_Pixel color) {
  u32 rgba = SDL_MapRGBA(canvas->format, color.r, color.g, color.b, color.a);
  canvas->userdata = (void *)(uword_t)rgba;
}

void owl_clear(owl_Canvas *canvas) {
  owl_fill(canvas, 0, 0, canvas->w, canvas->h);
}

void owl_fill(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h) {
  SDL_Rect rect = {x, y, w, h};
  SDL_FillRect(canvas, &rect, owl_getcolor(canvas));
}

void owl_line(owl_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2) {
  owl_Point line[2] = {{x1, y1}, {x2, y2}};
  owl_lines(canvas, line, 2);
}

void owl_lines(owl_Canvas *canvas, const owl_Point *points, s32 n) {
  u32 color = owl_getcolor(canvas);
  const owl_Point *p1 = NULL, *p2 = NULL;
  s32 i = 0;

  owl_lock(canvas);
  while (i < n) {
    p2 = &points[i++];

    if (p1)
      owl_drawline(canvas, p1->x, p1->y, p2->x, p2->y, color);

    p1 = p2;
  }
  owl_unlock(canvas);
}

void owl_rect(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h) {
  owl_Point points[] = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}, {x, y}};
  owl_lines(canvas, points, 5);
}

void owl_fillrect(owl_Canvas *canvas, s32 x, s32 y, s32 w, s32 h) {
  u32 color = owl_getcolor(canvas);
  s32 i;

  owl_lock(canvas);
  for (i = 0; i < h; ++i)
    owl_drawline(canvas, x, y + i, x + w, y + i, color);
  owl_unlock(canvas);
}

void owl_ellipse(owl_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry) {
  s32 dx, dy, d1, d2, rx2, ry2, offsetx, offsety;
  u32 color = owl_getcolor(canvas);

  offsetx = 0;
  offsety = ry;

  rx2 = rx * rx;
  ry2 = ry * ry;

  d1 = ry2 - (rx2 * ry) + (s32)ceil(0.25 * rx2);
  dx = 2 * ry2 * offsetx;
  dy = 2 * rx2 * offsety;

  owl_lock(canvas);

  while (dx < dy) {
    owl_drawpixel(canvas, x + offsetx, y + offsety, color);
    owl_drawpixel(canvas, x - offsetx, y - offsety, color);

    if (x + offsetx != x - offsetx) {
      owl_drawpixel(canvas, x - offsetx, y + offsety, color);
      owl_drawpixel(canvas, x + offsetx, y - offsety, color);
    }

    if (d1 < 0) {
      offsetx++;
      dx += ry2 << 1;
      d1 += dx + ry2;
    } else {
      offsetx++;
      offsety--;
      dx += ry2 << 1;
      dy -= rx2 << 1;
      d1 += dx - dy + ry2;
    }
  }

  d2 = (ry2 * (s32)ceil((offsetx + 0.5) * (offsetx + 0.5))) +
       (rx2 * ((offsety - 1) * (offsety - 1))) - (rx2 * ry2);

  while (offsety >= 0) {
    owl_drawpixel(canvas, x + offsetx, y + offsety, color);
    owl_drawpixel(canvas, x - offsetx, y - offsety, color);

    if (y + offsety != y - offsety) {
      owl_drawpixel(canvas, x - offsetx, y + offsety, color);
      owl_drawpixel(canvas, x + offsetx, y - offsety, color);
    }

    if (d2 > 0) {
      offsety--;
      dy -= rx2 << 1;
      d2 += rx2 - dy;
    } else {
      offsety--;
      offsetx++;
      dx += ry2 << 1;
      dy -= rx2 << 1;
      d2 += dx - dy + rx2;
    }
  }

  owl_unlock(canvas);
}

void owl_fillellipse(owl_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry) {
  s32 dx, dy, d1, d2, rx2, ry2, offx, offy;
  u32 color = owl_getcolor(canvas);

  offx = 0;
  offy = ry;

  rx2 = rx * rx;
  ry2 = ry * ry;

  d1 = ry2 - (rx2 * ry) + (s32)ceil(0.25 * rx2);
  dx = 2 * ry2 * offx;
  dy = 2 * rx2 * offy;

  owl_lock(canvas);

  while (dx < dy) {
    if (d1 < 0) {
      offx++;
      dx += ry2 << 1;
      d1 += dx + ry2;
    } else {
      owl_drawline(canvas, x - offx, y + offy, x + offx, y + offy, color);
      owl_drawline(canvas, x - offx, y - offy, x + offx, y - offy, color);

      offx++;
      offy--;
      dx += ry2 << 1;
      dy -= rx2 << 1;
      d1 += dx - dy + ry2;
    }
  }

  d2 = (ry2 * (s32)ceil((offx + 0.5) * (offx + 0.5))) +
       (rx2 * ((offy - 1) * (offy - 1))) - (rx2 * ry2);

  while (offy > 0) {
    owl_drawline(canvas, x - offx, y + offy, x + offx, y + offy, color);
    owl_drawline(canvas, x - offx, y - offy, x + offx, y - offy, color);

    if (d2 > 0) {
      offy--;
      dy -= rx2 << 1;
      d2 += rx2 - dy;
    } else {
      offy--;
      offx++;
      dx += ry2 << 1;
      dy -= rx2 << 1;
      d2 += dx - dy + rx2;
    }
  }

  owl_drawline(canvas, x - rx, y, x + rx, y, color);
  owl_unlock(canvas);
}

void owl_clip(owl_Canvas *canvas, const owl_Rect *rect) {
  SDL_SetClipRect(canvas, (const SDL_Rect *)rect);
}

void owl_blit(owl_Canvas *canvas, owl_Canvas *src, const owl_Rect *srcrect,
              const owl_Rect *dstrect) {
  SDL_BlitScaled(src, (SDL_Rect *)srcrect, canvas, (SDL_Rect *)dstrect);
}
