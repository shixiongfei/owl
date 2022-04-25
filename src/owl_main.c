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

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_main.h"
#include "actor.h"
#include "nio4c.h"
#include "owl.h"
#include "owlvm.h"

typedef struct Args {
  int retval;
  int argc;
  char **argv;
} Args;

static void owl_fillellipse(f32 x, f32 y, f32 rx, f32 ry) {
  f32 dx, dy, d1, d2, rx2, ry2, offx, offy;

  offx = 0;
  offy = ry;

  rx2 = rx * rx;
  ry2 = ry * ry;

  d1 = ry2 - (rx2 * ry) + 0.25f * rx2;
  dx = 2 * ry2 * offx;
  dy = 2 * rx2 * offy;

  while (dx < dy) {
    if (d1 < 0) {
      offx++;
      dx += ry2 * 2.0f;
      d1 += dx + ry2;
    } else {
      owl_line(x - offx, y + offy, x + offx, y + offy);
      owl_line(x - offx, y - offy, x + offx, y - offy);

      offx++;
      offy--;
      dx += ry2 * 2.0f;
      dy -= rx2 * 2.0f;
      d1 += dx - dy + ry2;
    }
  }

  d2 = (ry2 * (offx + 0.5f) * (offx + 0.5f)) +
       (rx2 * ((offy - 1.0f) * (offy - 1.0f))) - (rx2 * ry2);

  while (offy > 0) {
    owl_line(x - offx, y + offy, x + offx, y + offy);
    owl_line(x - offx, y - offy, x + offx, y - offy);

    if (d2 > 0) {
      offy--;
      dy -= rx2 * 2.0f;
      d2 += rx2 - dy;
    } else {
      offy--;
      offx++;
      dx += ry2 * 2.0f;
      dy -= rx2 * 2.0f;
      d2 += dx - dy + rx2;
    }
  }

  owl_line(x - rx, y, x + rx, y);
}

static int owl_main(int argc, char *argv[]) {
  bool quit = false;
  char title[128];
  owl_Event event;
  owl_Canvas *screen, *hero, *morph, *text1, *text2, *text3;
  owl_Point points[4] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  owl_Point lines[4] = {{320, 200}, {300, 240}, {340, 240}, {320, 200}};
  owl_Rect rect = {150, 150, 500, 50};
  owl_Rect rects[2] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  owl_Rect rects1[2] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  owl_Rect hero_pos, clip_text = {200, 50, 110, 12};
  owl_Rect text1_pos = {200, 20, -1, -1};
  owl_Rect text2_pos = {clip_text.x, clip_text.y, -1, -1};
  owl_Rect text3_pos = {15, 20, -1, -1};
  owl_Rect morph_pos = {550, 10, 200, 200};
  s32 w, h;
  f64 angle = 0.0;
  const char *text = "中英文abc混合ABC测试!";
  const s32 SCREEN_W = 800;
  const s32 SCREEN_H = 600;
  const u8 *kbd;
  owl_Matrix m;
  owl_Vector2 c = {80, 80}, v = {130, 80};

  owl_matrix_settransrotate(&m, c.x, c.y, owl_radians(1.0f));

  sprintf(title, "Think Pixels | Owl Core: %s VM: %s",
          owl_version(NULL, NULL, NULL), owlvm_version(NULL, NULL, NULL));

  if (!owl_init(SCREEN_W, SCREEN_H, title, 0))
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

  owl_size(hero, &w, &h);
  hero_pos.w = (f32)w;
  hero_pos.h = (f32)h;

  owl_textinput(true);
  owl_textinputposition(100, 100);

  if (owl_font("Unifont", 16)) {
    text1 = owl_text(text, owl_rgb(0xff, 0, 0));
    owl_size(text1, &w, &h);
    text1_pos.w = (f32)w;
    text1_pos.h = (f32)h;

    text2 = owl_text(text, owl_rgb(0, 0xff, 0xff));
    owl_size(text2, &w, &h);
    text2_pos.w = (f32)w;
    text2_pos.h = (f32)h;

    text3 = owl_text(text, owl_rgb(0xff, 0xff, 0));
    owl_size(text3, &w, &h);
    text3_pos.w = (f32)w;
    text3_pos.h = (f32)h;
  }

  while (!quit) {
    owl_color(owl_rgb(0, 0, 0));
    owl_clear();

    owl_color(owl_rgb(0xff, 0, 0xff));

    while (owl_event(&event)) {
      switch (event.type) {
      case OWL_EVENT_QUIT:
        quit = true;
        break;
      case OWL_EVENT_MOUSEMOVE:
        owl_pixel((f32)event.mouse.x, (f32)event.mouse.y);
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

    kbd = owl_keystate();

    if (kbd[OWL_KEY_APPLICATION])
      owl_pixel(50, 20);

    if (kbd[OWL_KEY_RIGHTBUTTON])
      owl_pixel(55, 20);

    owl_matrix_apply(&m, &v, v.x, v.y);
    owl_line((f32)c.x, (f32)c.y, (f32)v.x, (f32)v.y);

    owl_color(owl_rgb(0xff, 0, 0));
    owl_pixel(10, 10);
    owl_pixels(points, 4);

    owl_color(owl_rgb(0, 0xff, 0));
    owl_line(100, 20, 150, 20);
    owl_lines(lines, 4);

    owl_color(owl_rgb(255, 174, 201));
    owl_rect(rect.x, rect.y, rect.w, rect.h);
    owl_fillrect(rects[0].x, rects[0].y, rects[0].w, rects[0].h);
    owl_fillrect(rects[1].x, rects[1].y, rects[1].w, rects[1].h);

    owl_color(owl_rgba(0, 0xff, 0, 0x5f));
    owl_fillrect(rects1[0].x, rects1[0].y, rects1[0].w, rects1[0].h);
    owl_fillrect(rects1[1].x, rects1[1].y, rects1[1].w, rects1[1].h);

    owl_blit(hero, NULL, &hero_pos, angle, NULL, 0);

    angle += 1.0;

    if (angle >= 360.0)
      angle -= 360.0;

    owl_blit(text1, NULL, &text1_pos, 0, NULL, 0);

    owl_clip(&clip_text);
    owl_blit(text2, NULL, &text2_pos, 0, NULL, 0);
    owl_clip(NULL);

    owl_target(morph);
    owl_color(owl_rgb(128, 128, 128));
    owl_clear();
    owl_blit(text3, NULL, &text3_pos, 0, NULL, 0);

    owl_blendmode(morph, OWL_BLEND_NONE);
    owl_color(owl_rgba(0, 0, 0, 0));
    owl_fillrect(50, 50, 50, 50);
    owl_blendmode(morph, OWL_BLEND_ALPHA);

    owl_target(NULL);
    owl_blit(morph, NULL, &morph_pos, 0, NULL, 0);

    owl_color(owl_rgba(0, 0xff, 0, 0x5f));
    owl_fillrect(600, 450, 100, 50);
    owl_fillellipse(450, 450, 100, 50);

    owl_blendmode(screen, OWL_BLEND_NONE);
    owl_color(owl_rgba(0, 0, 0, 0));
    owl_fillellipse(450, 450, 90, 40);
    owl_blendmode(screen, OWL_BLEND_ALPHA);

    owl_present();
    owl_wait();
  }

  owl_freecanvas(hero);

  owl_quit();
  return 0;
}

static void actor_main(void *param) {
  Args *args = (Args *)param;
  args->retval = owl_main(args->argc, args->argv);
}

int main(int argc, char *argv[]) {
  Args args = {-1, argc, argv};

  nio_initialize(NULL);
  actor_initialize();
  actor_wrap(actor_main, &args);
  actor_finalize();
  nio_finalize();

  return args.retval;
}
