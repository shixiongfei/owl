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

static int owl_main(int argc, char *argv[]) {
  bool quit = false;
  char title[128];
  owl_Event event;
  owl_Canvas *screen, *hero, *morph, *text1, *text2, *text3;
  owl_Point points[] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  owl_Point lines[] = {{320, 200}, {300, 240}, {340, 240}};
  owl_Rect rect = {150, 150, 500, 50};
  owl_Rect rects[] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  owl_Rect rects1[] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  owl_Rect hero_pos, clip_text = {200, 50, 110, 12};
  owl_Rect text1_pos = {200, 20, -1, -1};
  owl_Rect text2_pos = {clip_text.x, clip_text.y, -1, -1};
  owl_Rect text3_pos = {15, 20, -1, -1};
  owl_Rect morph_pos = {550, 10, 200, 200};
  s32 i, w, h;
  f32 angle = 0.0f;
  const char *text = "中英文abc混合ABC测试!";
  const s32 SCREEN_W = 800;
  const s32 SCREEN_H = 600;
  const u8 *kbd;
  owl_Matrix m;
  owl_Vector2 c = {80, 80}, v = {130, 80};
  owl_Vertex vert[4] = {0};
  u16 indices[] = {0, 1, 2, 1, 2, 3};

  owl_matrixSetTransRotate(&m, c.x, c.y, owl_radians(1.0f));

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

  owl_loadFont("Unifont", "./unifont.ttf");

  owl_loadSound("coin1", "./coin1.wav");
  owl_loadSound("coin2", "./coin2.wav");

  hero = owl_loadex("hero.bmp", owl_rgb(0xff, 0, 0xff));

  hero_pos.x = 20;
  hero_pos.y = 200;

  owl_size(hero, &w, &h);
  hero_pos.w = (f32)w;
  hero_pos.h = (f32)h;

  owl_textInput(true);
  owl_textInputPosition(100, 100);

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

  // left-top
  vert[0].position.x = 200;
  vert[0].position.y = 100;
  vert[0].color.r = 255;
  vert[0].color.g = 255;
  vert[0].color.b = 255;
  vert[0].color.a = 255;
  vert[0].uv.x = 0.0f;
  vert[0].uv.y = 0.0f;

  // right-top
  vert[1].position.x = 200 + hero_pos.w;
  vert[1].position.y = 100;
  vert[1].color.r = 255;
  vert[1].color.g = 0;
  vert[1].color.b = 0;
  vert[1].color.a = 255;
  vert[1].uv.x = 1.0f;
  vert[1].uv.y = 0.0f;

  // left-bottom
  vert[2].position.x = 200;
  vert[2].position.y = 100 + hero_pos.h;
  vert[2].color.r = 0;
  vert[2].color.g = 0;
  vert[2].color.b = 255;
  vert[2].color.a = 255;
  vert[2].uv.x = 0.0f;
  vert[2].uv.y = 1.0f;

  // right-bottom
  vert[3].position.x = 200 + hero_pos.w;
  vert[3].position.y = 100 + hero_pos.h;
  vert[3].color.r = 0;
  vert[3].color.g = 255;
  vert[3].color.b = 0;
  vert[3].color.a = 255;
  vert[3].uv.x = 1.0f;
  vert[3].uv.y = 1.0f;

  owl_thickness(2.0f);

  while (!quit) {
    owl_color(owl_rgb(0, 0, 0));
    owl_clear();

    owl_color(owl_rgb(0xff, 0, 0xff));
    owl_geometry(hero, vert, sizeof(vert) / sizeof(*vert), indices,
                 sizeof(indices) / sizeof(*indices));

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

    owl_matrixApply(&m, &v, v.x, v.y);
    owl_line((f32)c.x, (f32)c.y, (f32)v.x, (f32)v.y);

    owl_color(owl_rgb(0xff, 0, 0));
    owl_pixel(10, 10);

    for (i = 0; i < 4; ++i)
      owl_pixel(points[i].x, points[i].y);

    owl_color(owl_rgb(0, 0xff, 0));
    owl_line(100, 20, 150, 20);

    owl_color(owl_rgb(0xff, 0xff, 0));
    owl_fillPolygon(lines, 3);

    owl_color(owl_rgb(0xff, 0, 0xff));
    owl_polygon(lines, 3, true);

    owl_color(owl_rgb(255, 174, 201));
    owl_rect(rect.x, rect.y, rect.w, rect.h);
    owl_fillRect(rects[0].x, rects[0].y, rects[0].w, rects[0].h);
    owl_fillRect(rects[1].x, rects[1].y, rects[1].w, rects[1].h);

    owl_color(owl_rgba(0, 0xff, 0, 0x5f));
    owl_fillRect(rects1[0].x, rects1[0].y, rects1[0].w, rects1[0].h);
    owl_fillRect(rects1[1].x, rects1[1].y, rects1[1].w, rects1[1].h);

    owl_blit(hero, NULL, &hero_pos, angle, NULL, 0);

    angle += 1.0f;

    if (angle >= 360.0f)
      angle -= 360.0f;

    owl_blit(text1, NULL, &text1_pos, 0, NULL, 0);

    owl_clip(&clip_text);
    owl_blit(text2, NULL, &text2_pos, 0, NULL, 0);
    owl_clip(NULL);

    owl_target(morph);
    owl_color(owl_rgb(128, 128, 128));
    owl_clear();
    owl_blit(text3, NULL, &text3_pos, 0, NULL, 0);

    owl_blendMode(morph, OWL_BLEND_NONE);
    owl_color(owl_rgba(0, 0, 0, 0));
    owl_fillRect(50, 50, 50, 50);
    owl_blendMode(morph, OWL_BLEND_ALPHA);

    owl_target(NULL);
    owl_blit(morph, NULL, &morph_pos, 0, NULL, 0);

    owl_color(owl_rgba(0, 0xff, 0, 0x5f));
    owl_fillRect(600, 450, 100, 50);
    owl_fillEllipse(450, 450, 100, 50, 0.0f);

    owl_blendMode(screen, OWL_BLEND_NONE);
    owl_color(owl_rgba(0, 0, 0, 0));
    owl_fillEllipse(450, 450, 90, 40, 0.0f);
    owl_blendMode(screen, OWL_BLEND_ALPHA);

    owl_present();
    owl_wait();
  }

  owl_freeCanvas(hero);

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
