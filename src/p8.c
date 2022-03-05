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

#include "p8.h"

int p8_main(int argc, char *argv[]) {
  if (p8_window_create(800, 600, "P8-2D", 0) < 0)
    return -1;

  while (!p8_window_closed()) {
    p8_window_update();
  }

  p8_window_destroy();
  return 0;
}

int main(int argc, char *argv[]) { return p8_main(argc, argv); }
