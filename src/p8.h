/*
 * p8.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#ifndef __P8_H__
#define __P8_H__

#include "p8_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

P8_INLINE p8_Pixel p8_rgb(u8 r, u8 g, u8 b) {
  p8_Pixel p = {r, g, b, 0xFF};
  return p;
}

P8_INLINE p8_Pixel p8_rgba(u8 r, u8 g, u8 b, u8 a) {
  p8_Pixel p = {r, g, b, a};
  return p;
}

P8_API u64 p8_ticks(void);
P8_API void p8_sleep(u32 ms);

P8_API bool p8_init(s32 w, s32 h, const char *title, s32 flags);
P8_API void p8_quit(void);

P8_API bool p8_setfps(u32 rate);
P8_API u32 p8_getfps(void);
P8_API u32 p8_wait(void);

P8_API bool p8_event(p8_Event *event);
P8_API const u8 *p8_keyboard(void);
P8_API u32 p8_mouse(s32 *x, s32 *y);

P8_API void p8_textinput(bool onoff);
P8_API bool p8_textinputactive(void);
P8_API bool p8_textinputshown(void);
P8_API void p8_textinputposition(s32 x, s32 y);

P8_API p8_Canvas *p8_canvas(s32 w, s32 h);
P8_API p8_Canvas *p8_image(const u8 *data, s32 w, s32 h, u8 format);
P8_API p8_Canvas *p8_imagex(const u8 *data, s32 w, s32 h, p8_Pixel colorkey);
P8_API p8_Canvas *p8_load(const char *filename);
P8_API p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey);
P8_API void p8_destroy(p8_Canvas *canvas);

P8_API void p8_size(p8_Canvas *canvas, s32 *w, s32 *h);

P8_API void p8_clear(p8_Canvas *canvas);
P8_API void p8_color(p8_Canvas *canvas, p8_Pixel color);

P8_API void p8_pixel(p8_Canvas *canvas, s32 x, s32 y);
P8_API void p8_pixels(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_line(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2);
P8_API void p8_lines(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_rect(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_rects(p8_Canvas *canvas, const p8_Rect *rects, s32 n);

P8_API void p8_fillrect(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_fillrects(p8_Canvas *canvas, const p8_Rect *rects, s32 n);

P8_API void p8_clip(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_blit(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
                    const p8_Rect *dstrect);
P8_API void p8_blitex(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
                      const p8_Rect *dstrect, f64 angle, const p8_Point *center,
                      u32 flip);
P8_API void p8_present(p8_Canvas *screen);

P8_API p8_Table *p8_table(void);
P8_API void p8_freetable(p8_Table *table, p8_Dtor dtor);
P8_API void p8_cleartable(p8_Table *table, p8_Dtor dtor);
P8_API s32 p8_tablesize(p8_Table *table);
P8_API void *p8_settable(p8_Table *table, const char *name, void *value);
P8_API void *p8_gettable(p8_Table *table, const char *name);
P8_API void *p8_isettable(p8_Table *table, u64 key, void *value);
P8_API void *p8_igettable(p8_Table *table, u64 key);

P8_API bool p8_loadfont(const char *name, const char *filename);
P8_API bool p8_font(const char *name, s32 size);
P8_API void p8_text(p8_Canvas *canvas, const char *text, s32 x, s32 y,
                    p8_Pixel color);
P8_API s32 p8_textwidth(const char *text);

P8_API p8_Audio p8_audio(s32 freq, u8 format, u8 channels, u16 samples);
P8_API void p8_closeaudio(p8_Audio audio);
P8_API void p8_clearaudio(p8_Audio audio);
P8_API void p8_playaudio(p8_Audio audio, bool onoff);
P8_API bool p8_putaudio(p8_Audio audio, const void *buffer, s32 len);
P8_API u32 p8_audiobuffered(p8_Audio audio);

P8_API bool p8_loadsound(const char *name, const char *filename);
P8_API bool p8_playing(const char *name);
P8_API bool p8_play(const char *name);
P8_API bool p8_stop(const char *name);
P8_API bool p8_pause(const char *name);
P8_API bool p8_resume(const char *name);

P8_API s64 p8_filesize(const char *filename);
P8_API u8 *p8_readfile(const char *filename);

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
