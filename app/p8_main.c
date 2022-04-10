/*
 * p8_main.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_main.h"
#include "p8.h"

#ifndef _WIN32
#define dynarray(type, name, size) type name[size]
#else
#include <malloc.h>
#define dynarray(type, name, size)                                             \
  type *name = (type *)_alloca((size) * sizeof(type))
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define pathdup(path) p8_pathformat(strdup(path))

static void message_box(s32 type, const char *title, const char *format, ...) {
  p8_MsgBoxButton mbtn = {P8_MSGBOX_RETURNKEY | P8_MSGBOX_ESCKEY, 0, "OK"};
  va_list args, ap;
  s32 nbytes;

  va_start(args, format);
  va_copy(ap, args);

  nbytes = vsnprintf(NULL, 0, format, ap);

  if (nbytes > 0) {
    dynarray(char, message, nbytes + 1);
    vsnprintf(message, nbytes + 1, format, args);
    p8_msgbox(type, title, message, &mbtn, 1);
  }
  va_end(args);
}

#define error_box(format, ...)                                                 \
  message_box(P8_MSGBOX_ERROR, "Error!", format, ##__VA_ARGS__)

static s32 show_version(void) {
  message_box(P8_MSGBOX_INFORMATION, "P8 Game Engine",
              "P8 v%s\nBuild: %s %s\n\nBy: %s", P8_RELEASE, __DATE__, __TIME__,
              P8_AUTHOR);
  return 0;
}

static s32 run(void) {
  bool quit = false;
  p8_Event event;
  p8_Canvas *screen, *hero, *morph;
  p8_Point points[4] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  p8_Point lines[4] = {{320, 200}, {300, 240}, {340, 240}, {320, 200}};
  p8_Rect rect = {150, 150, 500, 50};
  p8_Rect rects[2] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  p8_Rect rects1[2] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  p8_Rect hero_pos, clip_text = {200, 50, 110, 12};
  p8_Rect morph_pos = {550, 10, 200, 200};
  const char *text = "中英文abc混合ABC测试!";
  const s32 SCREEN_W = 800;
  const s32 SCREEN_H = 600;

  if (!p8_init(SCREEN_W, SCREEN_H, "Think Pixels", 0))
    return -1;

  screen = p8_screen();

  if (!screen) {
    p8_quit();
    return -1;
  }

  morph = p8_canvas(200, 200);

  p8_loadfont("Unifont", "./unifont.ttf");

  p8_loadsound("coin1", "./coin1.wav");
  p8_loadsound("coin2", "./coin2.wav");

  hero = p8_loadex("hero.bmp", p8_rgb(0xff, 0, 0xff));

  hero_pos.x = 20;
  hero_pos.y = 200;
  p8_size(hero, &hero_pos.w, &hero_pos.h);

  p8_textinput(true);
  p8_textinputposition(100, 100);

  while (!quit) {
    p8_color(screen, p8_rgb(0, 0, 0));
    p8_clear(screen);

    p8_color(screen, p8_rgb(0xff, 0, 0xff));

    while (p8_event(&event)) {
      switch (event.type) {
      case P8_EVENT_QUIT:
        quit = true;
        break;
      case P8_EVENT_MOUSEMOVE:
        p8_pixel(screen, event.mouse.x, event.mouse.y);
        break;
      case P8_EVENT_KEYUP:
        if (event.key.code == P8_KEY_K)
          p8_play("coin1");

        if (event.key.code == P8_KEY_L)
          p8_play("coin2");

        if (event.key.code == P8_KEY_J)
          p8_play("ghsy");

        if (event.key.code == P8_KEY_H)
          p8_play("test");
        break;
      }
    }

    p8_color(screen, p8_rgb(0xff, 0, 0));
    p8_pixel(screen, 10, 10);
    p8_pixels(screen, points, 4);

    p8_color(screen, p8_rgb(0, 0xff, 0));
    p8_line(screen, 100, 20, 150, 20);
    p8_lines(screen, lines, 4);

    p8_color(screen, p8_rgb(255, 174, 201));
    p8_rect(screen, rect.x, rect.y, rect.w, rect.h);
    p8_fillrect(screen, rects[0].x, rects[0].y, rects[0].w, rects[0].h);
    p8_fillrect(screen, rects[1].x, rects[1].y, rects[1].w, rects[1].h);

    p8_color(screen, p8_rgba(0, 0xff, 0, 0x5f));
    p8_fillrect(screen, rects1[0].x, rects1[0].y, rects1[0].w, rects1[0].h);
    p8_fillrect(screen, rects1[1].x, rects1[1].y, rects1[1].w, rects1[1].h);

    p8_blit(screen, hero, NULL, &hero_pos);

    if (p8_font("Unifont", 16)) {
      p8_text(screen, text, 200, 20, p8_rgb(0xff, 0, 0));

      p8_clip(screen, &clip_text);
      p8_text(screen, text, clip_text.x, clip_text.y, p8_rgb(0, 0xff, 0xff));
      p8_clip(screen, NULL);

      p8_color(morph, p8_rgb(128, 128, 128));
      p8_clear(morph);
      p8_text(morph, text, 15, 20, p8_rgb(0xff, 0xff, 0));

      p8_blendmode(morph, P8_BLEND_NONE);
      p8_color(morph, p8_rgba(0, 0, 0, 0));
      p8_fillrect(morph, 50, 50, 50, 50);
      p8_blendmode(morph, P8_BLEND_ALPHA);

      p8_blit(screen, morph, NULL, &morph_pos);
    }

    p8_fillrect(screen, 600, 450, 100, 50);
    p8_fillellipse(screen, 450, 450, 100, 50);

    p8_blendmode(screen, P8_BLEND_NONE);
    p8_color(screen, p8_rgba(0, 0, 0, 0));
    p8_fillellipse(screen, 450, 450, 90, 40);
    p8_blendmode(screen, P8_BLEND_ALPHA);

    p8_present();
    p8_wait();
  }

  p8_destroy(hero);

  p8_quit();
  return 0;
}

int main(int argc, char *argv[]) {
  run();
  return 0;
}
