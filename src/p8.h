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

P8_API bool p8_init(s32 width, s32 height, const char *title, s32 flags);
P8_API void p8_quit(void);

P8_API bool p8_setfps(u32 rate);
P8_API u32 p8_getfps(void);
P8_API u32 p8_wait(void);

P8_API s32 p8_msgbox(s32 type, const char *title, const char *message,
                     const p8_MsgBoxButton *buttons, s32 count);

P8_API bool p8_event(p8_Event *event);
P8_API const u8 *p8_keyboard(void);
P8_API u32 p8_mouse(s32 *x, s32 *y);

P8_API void p8_textinput(bool onoff);
P8_API bool p8_textinputactive(void);
P8_API bool p8_textinputshown(void);
P8_API void p8_textinputposition(s32 x, s32 y);

P8_API p8_Canvas *p8_screen(void);
P8_API p8_Canvas *p8_canvas(s32 width, s32 height);
P8_API p8_Canvas *p8_image(const u8 *data, s32 w, s32 h, u8 format);
P8_API p8_Canvas *p8_imagex(const u8 *data, s32 w, s32 h, p8_Pixel colorkey);
P8_API p8_Canvas *p8_load(const char *filename);
P8_API p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey);
P8_API void p8_destroy(p8_Canvas *canvas);

P8_API void p8_size(p8_Canvas *canvas, s32 *w, s32 *h);
P8_API void p8_blendmode(p8_Canvas *canvas, s32 mode);

P8_API void p8_color(p8_Canvas *canvas, p8_Pixel color);
P8_API void p8_clear(p8_Canvas *canvas);

P8_API void p8_pixel(p8_Canvas *canvas, s32 x, s32 y);
P8_API void p8_pixels(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_line(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2);
P8_API void p8_lines(p8_Canvas *canvas, const p8_Point *points, s32 n);

P8_API void p8_rect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h);
P8_API void p8_fillrect(p8_Canvas *canvas, s32 x, s32 y, s32 w, s32 h);

P8_API void p8_ellipse(p8_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry);
P8_API void p8_fillellipse(p8_Canvas *canvas, s32 x, s32 y, s32 rx, s32 ry);

P8_API void p8_arc(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
                   f32 theta);
P8_API void p8_pie(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
                   f32 theta);
P8_API void p8_fillpie(p8_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
                       f32 theta);

P8_API void p8_clip(p8_Canvas *canvas, const p8_Rect *rect);
P8_API void p8_blit(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
                    const p8_Rect *dstrect);
P8_API void p8_present(void);

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
P8_API s32 p8_text(p8_Canvas *canvas, const char *text, s32 x, s32 y,
                   p8_Pixel color);
P8_API s32 p8_textwidth(const char *text);

P8_API p8_Audio p8_audio(s32 freq, u8 format, u8 channels, u16 samples);
P8_API void p8_closeaudio(p8_Audio audio);
P8_API void p8_clearaudio(p8_Audio audio);
P8_API void p8_playaudio(p8_Audio audio, bool onoff);
P8_API bool p8_audiostream(p8_Audio audio, const void *buffer, s32 len);
P8_API u32 p8_audiobuffered(p8_Audio audio);

P8_API bool p8_loadsound(const char *name, const char *filename);
P8_API bool p8_playing(const char *name);
P8_API bool p8_play(const char *name);
P8_API bool p8_stop(const char *name);
P8_API bool p8_pause(const char *name);
P8_API bool p8_resume(const char *name);

P8_API s64 p8_filesize(const char *filename);
P8_API u8 *p8_readfile(const char *filename);
P8_API int p8_tempfile(const char *filename);

P8_API const char *p8_selfname(void);
P8_API char *p8_pathformat(char *path);
P8_API char *p8_dirname(char *outbuf, const char *path);
P8_API char *p8_basename(char *outbuf, const char *path);

P8_API s32 p8_pathtype(const char *path);
P8_API char *p8_resolvepath(char *outbuf, const char *path);

P8_API void p8_setcwd(const char *workdir);
P8_API char *p8_getcwd(char *workdir, s32 size);

P8_API bool p8_isexist(const char *path);
P8_API bool p8_isdir(const char *path);
P8_API bool p8_isfile(const char *path);
P8_API bool p8_islink(const char *path);

P8_API p8_Dylib p8_dylib(const char *sofile);
P8_API void *p8_dysym(p8_Dylib dylib, const char *name);

#if P8_WINDOWS
P8_API u32 p8_pesize(s64 *filesize);
#endif

#ifdef __cplusplus
};
#endif

#endif /* __P8_H__ */
