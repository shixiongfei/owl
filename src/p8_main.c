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

#include "p8.h"

#define SCREEN_W 800
#define SCREEN_H 600

int p8_main(int argc, char *argv[]) {
  p8_Canvas *screen, *hero;
  p8_Point points[4] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  p8_Point lines[4] = {{320, 200}, {300, 240}, {340, 240}, {320, 200}};
  p8_Rect rect = {150, 150, 100, 50};
  p8_Rect rects[2] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  p8_Rect rects1[2] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  p8_Rect hero_pos;

  if (!p8_init(SCREEN_W, SCREEN_H, "Think Pixels", 0))
    return -1;

  screen = p8_canvas(SCREEN_W, SCREEN_H);

  if (!screen) {
    p8_quit();
    return -1;
  }

  p8_loadfont("", "./simsun.ttc");

  hero = p8_loadex("hero.bmp", p8_rgb(0xff, 0, 0xff));

  hero_pos.x = 20;
  hero_pos.y = 200;
  p8_size(hero, &hero_pos.w, &hero_pos.h);

  while (!p8_closed()) {
    p8_events();

    p8_color(screen, p8_rgb(0, 0, 0));
    p8_clear(screen);

    p8_color(screen, p8_rgb(0xff, 0, 0));
    p8_pixel(screen, 10, 10);
    p8_pixels(screen, points, 4);

    p8_color(screen, p8_rgb(0, 0xff, 0));
    p8_line(screen, 100, 20, 150, 20);
    p8_lines(screen, lines, 4);

    p8_color(screen, p8_rgb(255, 174, 201));
    p8_rects(screen, &rect, 1);
    p8_fillrects(screen, rects, 2);

    p8_color(screen, p8_rgba(0, 0xff, 0, 0x5f));
    p8_fillrects(screen, rects1, 2);

    p8_blit(screen, hero, NULL, &hero_pos);

    p8_present(screen);
    p8_wait();
  }

  p8_destroy(hero);
  p8_destroy(screen);
  p8_quit();

  return 0;
}
