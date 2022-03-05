/*
 * p8_window.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#include "p8.h"

typedef struct p8_Window {
  void *screen;
} p8_Window;

static p8_Window p8_window = {0};

int p8_window_create(s32 w, s32 h, const char *title, int flags) { return 0; }

void p8_window_destroy(void) {}

int p8_window_closed(void) { return 1; }

void p8_window_update(void) {}
