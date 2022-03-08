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
  p8_Canvas *screen;

  if (!p8_init(SCREEN_W, SCREEN_H, "Think Pixels", 0))
    return -1;

  screen = p8_canvas(SCREEN_W, SCREEN_H);

  if (!screen) {
    p8_quit();
    return -1;
  }

  while (!p8_closed()) {
    p8_events();

    p8_clear(screen, p8_rgb(0, 0, 0));
    p8_pixel(screen, 10, 10, p8_rgb(0xff, 0, 0));
    p8_fillrect(screen, 15, 15, 100, 100, p8_rgb(0xff, 0xff, 0));
    p8_rect(screen, 20, 20, 100, 100, p8_rgb(0, 0, 0xff));
    p8_line(screen, 150, 10, 200, 15, p8_rgb(0xff, 0, 0));
    p8_aaline(screen, 150, 30, 200, 35, p8_rgb(0xff, 0, 0));
    p8_thickline(screen, 150, 50, 200, 55, 5, p8_rgb(0xff, 0, 0));
    p8_arc(screen, 150, 70, 50, 0, 80, p8_rgb(0xff, 0, 0));
    p8_fillroundedrect(screen, 30, 30, 100, 100, 10, p8_rgb(0, 0xff, 0xff));
    p8_roundedrect(screen, 50, 50, 100, 100, 10, p8_rgb(0xff, 0, 0));
    p8_circle(screen, 130, 130, 50, p8_rgb(0xff, 0, 0xff));
    p8_aacircle(screen, 160, 160, 50, p8_rgb(0xff, 0, 0xff));
    p8_fillcircle(screen, 190, 190, 50, p8_rgb(0xff, 0, 0xff));
    p8_ellipse(screen, 220, 220, 100, 50, p8_rgb(0, 0, 0xff));
    p8_aaellipse(screen, 250, 250, 100, 50, p8_rgb(0, 0, 0xff));
    p8_fillellipse(screen, 280, 280, 100, 50, p8_rgb(0, 0, 0xff));
    p8_pie(screen, 300, 300, 50, 20, 70, p8_rgb(0xff, 0, 0));
    p8_fillpie(screen, 330, 330, 50, 20, 70, p8_rgb(0xff, 0, 0));
    p8_trigon(screen, 350, 350, 400, 300, 400, 400, p8_rgb(0, 0, 0xff));
    p8_aatrigon(screen, 400, 380, 520, 380, 480, 480, p8_rgb(0, 0, 0xff));
    p8_filltrigon(screen, 450, 420, 510, 410, 510, 510, p8_rgb(0, 0, 0xff));

    p8_update(screen);
    p8_wait();
  }

  p8_destroy(screen);
  p8_quit();
  return 0;
}
