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
#define SCREEN_FPS 60

int p8_main(int argc, char *argv[]) {
  p8_Canvas *screen;

  if (!p8_init(SCREEN_W, SCREEN_H, "Think Pixels", SCREEN_FPS, 0))
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
    p8_fillroundedrect(screen, 30, 30, 100, 100, 10, p8_rgb(0, 0xff, 0xff));
    p8_roundedrect(screen, 50, 50, 100, 100, 10, p8_rgb(0xff, 0, 0));

    p8_update(screen);
  }

  p8_destroy(screen);
  p8_quit();
  return 0;
}
