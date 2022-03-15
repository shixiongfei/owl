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

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "p8.h"

#define P8_FPS_UPPER_LIMIT 100
#define P8_FPS_LOWER_LIMIT 1
#define P8_FPS_DEFAULT 60

#define P8_BLENDMODE SDL_BLENDMODE_BLEND

#define P8_SOUND_NONE 0
#define P8_SOUND_WAV 1
#define P8_SOUND_FLAC 2
#define P8_SOUND_MP3 3

typedef struct SDL_Surface p8_Image;
typedef struct stbtt_fontinfo p8_TrueType;

typedef struct p8_Font {
  p8_TrueType *ttf;
  s32 ascent;
  s32 descent;
  s32 linegap;
  f32 scale;
  f32 baseline;
  f32 height;
} p8_Font;

typedef struct p8_Sound {
  SDL_AudioSpec spec;
  p8_Audio audio;
  u32 type;
  u32 size;
  u8 *buffer;
} p8_Sound;

typedef struct p8_Window {
  SDL_Window *window;
  SDL_Renderer *renderer;

  u32 rate;
  u32 framecount;
  f32 rateticks;
  u64 baseticks;
  u64 lastticks;

  p8_Table *ttfs;
  p8_Font font;

  p8_Table *sounds;
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

static f32 p8_ttfwidth(p8_Font *font, const char *text) {
  const char *p = text;
  f32 width = 0;
  ucs4_t ch, last = 0;

  while (*p) {
    p += utf8_tounicode(p, &ch);
    width += p8_ttfwide(font, ch, last);
    last = ch;
  }
  return ceilf(width);
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

  *w = (s32)p8_ttfwidth(font, text);
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
    xoff += p8_ttfwide(font, ch, last);
    xfract = xoff - (int)xoff;
    last = ch;
  }
  return bitmap;
}

static p8_Audio p8_openaudio(const SDL_AudioSpec *spec) {
  p8_Audio audio = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0);

  if (audio)
    SDL_PauseAudioDevice(audio, 0);

  return audio;
}

static p8_Sound *p8_loadwav(const char *filename) {
  p8_Sound *sound;
  SDL_AudioSpec spec;
  u8 *wav;
  u32 size;

  if (!SDL_LoadWAV(filename, &spec, &wav, &size))
    return NULL;

  sound = (p8_Sound *)calloc(1, sizeof(p8_Sound));

  if (!sound) {
    SDL_FreeWAV(wav);
    return NULL;
  }

  sound->type = P8_SOUND_WAV;
  sound->spec = spec;
  sound->buffer = wav;
  sound->size = size;

  return sound;
}

static p8_Sound *p8_loadflac(const char *filename) {
  p8_Sound *sound;
  u32 channels, sample_rate;
  u64 num_samples;
  s32 *samples;

  samples = drflac_open_file_and_read_pcm_frames_s32(
      filename, &channels, &sample_rate, &num_samples, NULL);

  if (!samples)
    return NULL;

  sound = (p8_Sound *)calloc(1, sizeof(p8_Sound));

  if (!sound) {
    drflac_free(samples, NULL);
    return NULL;
  }

  sound->type = P8_SOUND_FLAC;
  sound->spec.freq = sample_rate;
  sound->spec.channels = channels;
  sound->spec.format = AUDIO_S32SYS;
  sound->spec.samples = 4096;
  sound->buffer = (u8 *)samples;
  sound->size = (u32)(num_samples * channels * sizeof(s32));

  return sound;
}

static p8_Sound *p8_loadmp3(const char *filename) {
  p8_Sound *sound;
  drmp3_config mp3;
  u64 num_samples;
  s16 *samples;

  samples = drmp3_open_file_and_read_pcm_frames_s16(filename, &mp3,
                                                    &num_samples, NULL);
  if (!samples)
    return NULL;

  sound = (p8_Sound *)calloc(1, sizeof(p8_Sound));

  if (!sound) {
    drmp3_free(samples, NULL);
    return NULL;
  }

  sound->type = P8_SOUND_MP3;
  sound->spec.freq = mp3.sampleRate;
  sound->spec.channels = mp3.channels;
  sound->spec.format = AUDIO_S16SYS;
  sound->spec.samples = 4096;
  sound->buffer = (u8 *)samples;
  sound->size = (u32)(num_samples * mp3.channels * sizeof(s16));

  return sound;
}

static p8_Sound *p8_sound(const char *filename) {
  p8_Sound *sound;

  sound = p8_loadwav(filename);

  if (sound)
    return sound;

  sound = p8_loadflac(filename);

  if (sound)
    return sound;

  sound = p8_loadmp3(filename);

  if (sound)
    return sound;

  return NULL;
}

static void p8_soundfree(p8_Sound *sound) {
  p8_clearaudio(sound->audio);
  p8_closeaudio(sound->audio);

  switch (sound->type) {
  case P8_SOUND_WAV:
    SDL_FreeWAV(sound->buffer);
    break;
  case P8_SOUND_FLAC:
    drflac_free(sound->buffer, NULL);
    break;
  case P8_SOUND_MP3:
    drmp3_free(sound->buffer, NULL);
    break;
  }

  free(sound);
}

u64 p8_ticks(void) {
  u64 ticks = SDL_GetTicks64();
  return ticks > 0 ? ticks : 1;
}

void p8_sleep(u32 ms) { SDL_Delay(ms); }

bool p8_init(s32 w, s32 h, const char *title, s32 flags) {
  s32 x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;
  s32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  app->window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_ALLOW_HIGHDPI);

  if (!app->window)
    goto error;

  app->renderer = SDL_CreateRenderer(app->window, -1, renderer_flags);

  if (!app->renderer)
    goto error;

  app->ttfs = p8_table();

  if (!app->ttfs)
    goto error;

  app->sounds = p8_table();

  if (!app->sounds)
    goto error;

  SDL_SetRenderDrawBlendMode(app->renderer, P8_BLENDMODE);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_DisableScreenSaver();

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
    p8_freetable(app->ttfs, (p8_Dtor)p8_ttffree);
    app->ttfs = NULL;
  }

  if (app->sounds) {
    p8_freetable(app->sounds, (p8_Dtor)p8_soundfree);
    app->sounds = NULL;
  }

  SDL_Quit();
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

bool p8_event(p8_Event *event) {
  SDL_Event e;

  memset(event, 0, sizeof(p8_Event));

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      event->type = P8_EVENT_QUIT;
      return true;

    case SDL_KEYDOWN:
      event->type = P8_EVENT_KEYDOWN;
      event->key.code = e.key.keysym.scancode;
      return true;

    case SDL_KEYUP:
      event->type = P8_EVENT_KEYUP;
      event->key.code = e.key.keysym.scancode;
      return true;

    case SDL_MOUSEMOTION:
      event->type = P8_EVENT_MOUSEMOVE;
      event->mouse.state = e.motion.state;
      event->mouse.x = e.motion.x;
      event->mouse.y = e.motion.y;
      event->mouse.xrel = e.motion.xrel;
      event->mouse.yrel = e.motion.yrel;
      return true;

    case SDL_MOUSEBUTTONDOWN:
      event->type = P8_EVENT_MOUSEDOWN;
      event->button.button = e.button.button;
      event->button.clicks = e.button.clicks;
      event->button.x = e.button.x;
      event->button.y = e.button.y;
      return true;

    case SDL_MOUSEBUTTONUP:
      event->type = P8_EVENT_MOUSEUP;
      event->button.button = e.button.button;
      event->button.clicks = e.button.clicks;
      event->button.x = e.button.x;
      event->button.y = e.button.y;
      return true;

    case SDL_MOUSEWHEEL:
      event->type = P8_EVENT_MOUSEWHEEL;
      event->wheel.x = e.wheel.preciseX;
      event->wheel.y = e.wheel.preciseY;
      return true;

    case SDL_TEXTINPUT:
      event->type = P8_EVENT_TEXTINPUT;
      memcpy(event->input.text, e.text.text, SDL_TEXTINPUTEVENT_TEXT_SIZE);
      return true;

    case SDL_TEXTEDITING:
      event->type = P8_EVENT_TEXTEDITING;
      event->edit.start = e.edit.start;
      event->edit.length = e.edit.length;
      memcpy(event->edit.text, e.edit.text, SDL_TEXTEDITINGEVENT_TEXT_SIZE);
      return true;
    }
  }
  return false;
}

const u8 *p8_keyboard(void) { return SDL_GetKeyboardState(NULL); }

u32 p8_mouse(s32 *x, s32 *y) { return SDL_GetMouseState(x, y); }

void p8_textinput(bool onoff) {
  if (onoff)
    SDL_StartTextInput();
  else
    SDL_StopTextInput();
}

bool p8_textinputactive(void) { return SDL_IsTextInputActive(); }

void p8_textinputrect(p8_Rect *rect) { SDL_SetTextInputRect((SDL_Rect *)rect); }

p8_Canvas *p8_canvas(s32 w, s32 h) {
  p8_Canvas *canvas = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32,
                                        SDL_TEXTUREACCESS_TARGET, w, h);
  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, P8_BLENDMODE);

  return canvas;
}

static p8_Canvas *p8_dynamic(s32 w, s32 h, u8 format) {
  s32 f = (format == P8_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                    : SDL_PIXELFORMAT_RGBA32;
  s32 a = SDL_TEXTUREACCESS_STREAMING;
  p8_Canvas *canvas = SDL_CreateTexture(app->renderer, f, a, w, h);

  if (!canvas)
    return NULL;

  SDL_SetTextureBlendMode(canvas, P8_BLENDMODE);

  return canvas;
}

static p8_Image *p8_toimage(s32 w, s32 h, const u8 *data, u8 format) {
  s32 d = format * 8;
  s32 p = format * w;
  s32 f = (format == P8_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                    : SDL_PIXELFORMAT_RGBA32;
  if (!data)
    return NULL;

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

p8_Canvas *p8_image(const u8 *data, s32 w, s32 h, u8 format) {
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
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = p8_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

p8_Canvas *p8_loadex(const char *filename, p8_Pixel colorkey) {
  p8_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

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

void p8_blitex(p8_Canvas *canvas, p8_Canvas *src, const p8_Rect *srcrect,
               const p8_Rect *dstrect, f64 angle, const p8_Point *center,
               u32 flip) {
  SDL_SetRenderTarget(app->renderer, canvas);
  SDL_RenderCopyEx(app->renderer, src, (const SDL_Rect *)srcrect,
                   (const SDL_Rect *)dstrect, angle, (const SDL_Point *)center,
                   flip);
}

void p8_present(p8_Canvas *screen) {
  SDL_SetRenderTarget(app->renderer, NULL);
  SDL_RenderCopy(app->renderer, screen, NULL, NULL);
  SDL_RenderPresent(app->renderer);
}

bool p8_loadfont(const char *name, const char *filename) {
  p8_TrueType *ttf = (p8_TrueType *)p8_gettable(app->ttfs, name);

  if (ttf)
    return true;

  if (!filename)
    return false;

  ttf = p8_loadttf(filename);

  if (!ttf)
    return false;

  p8_settable(app->ttfs, name, ttf);
  return true;
}

bool p8_font(const char *name, s32 size) {
  p8_TrueType *ttf = (p8_TrueType *)p8_gettable(app->ttfs, name);
  s32 ascent, descent, linegap;

  if (!ttf)
    return false;

  stbtt_GetFontVMetrics(ttf, &ascent, &descent, &linegap);

  app->font.ttf = ttf;
  app->font.ascent = ascent;
  app->font.descent = descent;
  app->font.linegap = linegap;
  app->font.scale = stbtt_ScaleForMappingEmToPixels(ttf, (f32)size);
  app->font.baseline = ceilf(ascent * app->font.scale);
  app->font.height = ceilf((ascent - descent) * app->font.scale);

  return true;
}

void p8_text(p8_Canvas *canvas, const char *text, s32 x, s32 y,
             p8_Pixel color) {
  p8_Font *font = &app->font;
  p8_Rect rect = {x, y, -1, -1};
  p8_Canvas *texture;
  s32 i, pitch;
  u32 *pixels;
  u8 *bitmap;

  if (!text)
    return;

  bitmap = p8_ttfbitmap(font, text, &rect.w, &rect.h);

  if (!bitmap)
    return;

  texture = p8_dynamic(rect.w, rect.h, P8_FORMAT_RGBA);

  if (!texture) {
    free(bitmap);
    return;
  }

  SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
  for (i = 0; i < rect.w * rect.h; ++i)
    pixels[i] = p8_rgba(0xFF, 0xFF, 0xFF, bitmap[i]).rgba;
  SDL_UnlockTexture(texture);
  free(bitmap);

  SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
  SDL_SetTextureAlphaMod(texture, color.a);

  p8_blit(canvas, texture, NULL, &rect);
  p8_destroy(texture);
}

s32 p8_textwidth(const char *text) {
  p8_Font *font = &app->font;

  if (!text)
    return -1;

  return (s32)p8_ttfwidth(font, text);
}

p8_Audio p8_audio(s32 freq, u8 format, u8 channels, u16 samples) {
  SDL_AudioSpec spec = {0};

  spec.freq = freq;
  spec.channels = channels;
  spec.samples = samples;

  switch (format) {
  case P8_AUDIO_U8:
    spec.format = AUDIO_U8;
    break;
  case P8_AUDIO_S8:
    spec.format = AUDIO_S8;
    break;
  case P8_AUDIO_U16:
    spec.format = AUDIO_U16SYS;
    break;
  case P8_AUDIO_S16:
    spec.format = AUDIO_S16SYS;
    break;
  case P8_AUDIO_S32:
    spec.format = AUDIO_S32SYS;
    break;
  case P8_AUDIO_F32:
    spec.format = AUDIO_F32SYS;
    break;
  default:
    return 0;
  }

  return p8_openaudio(&spec);
}

void p8_closeaudio(p8_Audio audio) {
  if (!audio)
    return;

  SDL_CloseAudioDevice(audio);
}

void p8_clearaudio(p8_Audio audio) {
  if (!audio)
    return;

  SDL_ClearQueuedAudio(audio);
}

void p8_playaudio(p8_Audio audio, bool onoff) {
  if (!audio)
    return;

  SDL_PauseAudioDevice(audio, !onoff);
}

bool p8_putaudio(p8_Audio audio, const void *buffer, s32 len) {
  if (!audio)
    return false;

  return 0 == SDL_QueueAudio(audio, buffer, len);
}

u32 p8_audiobuffered(p8_Audio audio) {
  if (!audio)
    return 0;

  return SDL_GetQueuedAudioSize(audio);
}

bool p8_loadsound(const char *name, const char *filename) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (sound)
    return true;

  if (!filename)
    return false;

  sound = p8_sound(filename);

  if (!sound)
    return false;

  p8_settable(app->sounds, name, sound);
  return true;
}

bool p8_playing(const char *name) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (!sound)
    return false;

  return p8_audiobuffered(sound->audio) > 0;
}

bool p8_play(const char *name) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (!sound)
    return false;

  if (!sound->audio)
    sound->audio = p8_openaudio(&sound->spec);

  return p8_putaudio(sound->audio, sound->buffer, sound->size);
}

bool p8_stop(const char *name) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (!sound)
    return false;

  p8_clearaudio(sound->audio);
  return true;
}

bool p8_pause(const char *name) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (!sound)
    return false;

  p8_playaudio(sound->audio, false);
  return true;
}

bool p8_resume(const char *name) {
  p8_Sound *sound = (p8_Sound *)p8_gettable(app->sounds, name);

  if (!sound)
    return false;

  p8_playaudio(sound->audio, true);
  return true;
}

extern int p8_main(int argc, char *argv[]);
int main(int argc, char *argv[]) { return p8_main(argc, argv); }
