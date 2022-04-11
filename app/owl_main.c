/*
 * owl_main.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_main.h"
#include "owl.h"

int main(int argc, char *argv[]) {
  bool quit = false;
  owl_Event event;
  owl_Canvas *screen, *hero, *morph;
  owl_Point points[4] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  owl_Point lines[4] = {{320, 200}, {300, 240}, {340, 240}, {320, 200}};
  owl_Rect rect = {150, 150, 500, 50};
  owl_Rect rects[2] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  owl_Rect rects1[2] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  owl_Rect hero_pos, clip_text = {200, 50, 110, 12};
  owl_Rect morph_pos = {550, 10, 200, 200};
  const char *text = "中英文abc混合ABC测试!";
  const s32 SCREEN_W = 800;
  const s32 SCREEN_H = 600;

  if (!owl_init(SCREEN_W, SCREEN_H, "Think Pixels", 0))
    return -1;

  screen = owl_screen();

  if (!screen) {
    owl_quit();
    return -1;
  }

  morph = owl_canvas(200, 200);

  owl_loadfont("Unifont", "./unifont.ttf");

  owl_loadsound("coin1", "./coin1.wav");
  owl_loadsound("coin2", "./coin2.wav");

  hero = owl_loadex("hero.bmp", owl_rgb(0xff, 0, 0xff));

  hero_pos.x = 20;
  hero_pos.y = 200;
  owl_size(hero, &hero_pos.w, &hero_pos.h);

  owl_textinput(true);
  owl_textinputposition(100, 100);

  while (!quit) {
    owl_color(screen, owl_rgb(0, 0, 0));
    owl_clear(screen);

    owl_color(screen, owl_rgb(0xff, 0, 0xff));

    while (owl_event(&event)) {
      switch (event.type) {
      case OWL_EVENT_QUIT:
        quit = true;
        break;
      case OWL_EVENT_MOUSEMOVE:
        owl_pixel(screen, event.mouse.x, event.mouse.y);
        break;
      case OWL_EVENT_KEYUP:
        if (event.key.code == OWL_KEY_K)
          owl_play("coin1");

        if (event.key.code == OWL_KEY_L)
          owl_play("coin2");

        if (event.key.code == OWL_KEY_J)
          owl_play("ghsy");

        if (event.key.code == OWL_KEY_H)
          owl_play("test");
        break;
      }
    }

    owl_color(screen, owl_rgb(0xff, 0, 0));
    owl_pixel(screen, 10, 10);
    owl_pixels(screen, points, 4);

    owl_color(screen, owl_rgb(0, 0xff, 0));
    owl_line(screen, 100, 20, 150, 20);
    owl_lines(screen, lines, 4);

    owl_color(screen, owl_rgb(255, 174, 201));
    owl_rect(screen, rect.x, rect.y, rect.w, rect.h);
    owl_fillrect(screen, rects[0].x, rects[0].y, rects[0].w, rects[0].h);
    owl_fillrect(screen, rects[1].x, rects[1].y, rects[1].w, rects[1].h);

    owl_color(screen, owl_rgba(0, 0xff, 0, 0x5f));
    owl_fillrect(screen, rects1[0].x, rects1[0].y, rects1[0].w, rects1[0].h);
    owl_fillrect(screen, rects1[1].x, rects1[1].y, rects1[1].w, rects1[1].h);

    owl_blit(screen, hero, NULL, &hero_pos);

    if (owl_font("Unifont", 16)) {
      owl_text(screen, text, 200, 20, owl_rgb(0xff, 0, 0));

      owl_clip(screen, &clip_text);
      owl_text(screen, text, clip_text.x, clip_text.y, owl_rgb(0, 0xff, 0xff));
      owl_clip(screen, NULL);

      owl_color(morph, owl_rgb(128, 128, 128));
      owl_clear(morph);
      owl_text(morph, text, 15, 20, owl_rgb(0xff, 0xff, 0));

      owl_blendmode(morph, OWL_BLEND_NONE);
      owl_color(morph, owl_rgba(0, 0, 0, 0));
      owl_fillrect(morph, 50, 50, 50, 50);
      owl_blendmode(morph, OWL_BLEND_ALPHA);

      owl_blit(screen, morph, NULL, &morph_pos);
    }

    owl_fillrect(screen, 600, 450, 100, 50);
    owl_fillellipse(screen, 450, 450, 100, 50);

    owl_blendmode(screen, OWL_BLEND_NONE);
    owl_color(screen, owl_rgba(0, 0, 0, 0));
    owl_fillellipse(screen, 450, 450, 90, 40);
    owl_blendmode(screen, OWL_BLEND_ALPHA);

    owl_present();
    owl_wait();
  }

  owl_destroy(hero);

  owl_quit();
  return 0;
}
