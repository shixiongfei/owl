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

#include "wren.h"

#include "p8.h"

#ifndef _WIN32
#define dynarray(type, name, size) type name[size]
#else
#include <malloc.h>
#define dynarray(type, name, size)                                             \
  type *name = (type *)_alloca((size) * sizeof(type))
#endif

static void message_box(s32 type, const char *title, const char *format, ...) {
  p8_MsgBoxButton mbtn = {P8_MSGBOX_RETURNKEY | P8_MSGBOX_ESCKEY, 0, "OK"};
  va_list args, ap;
  s32 nbytes;

  va_start(args, format);
  va_copy(ap, args);

  nbytes = vsnprintf(NULL, 0, format, ap);

  if (nbytes > 0) {
    dynarray(u8, message, nbytes + 1);
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

static u8 *load_source(const char *module) {
  char filename[260] = {0};

  snprintf(filename, sizeof(filename), "%s/main.wren", module);
  return p8_readfile(filename);
}

static void wren_onwrite(WrenVM *vm, const char *text) {
  if (0 == strcmp(text, "\n"))
    return;
  message_box(P8_MSGBOX_INFORMATION, "Message", "%s", text);
}

static void wren_onerror(WrenVM *vm, WrenErrorType type, const char *module,
                         const int line, const char *msg) {
  switch (type) {
  case WREN_ERROR_COMPILE:
    error_box("Compile Error: [%s line %d] [Error] %s", module, line, msg);
    break;
  case WREN_ERROR_STACK_TRACE:
    error_box("Stack Trace: [%s line %d] in %s", module, line, msg);
    break;
  case WREN_ERROR_RUNTIME:
    error_box("Runtime Error: [Runtime Error] %s", msg);
    break;
  }
}

static WrenVM *init_vm(void) {
  WrenConfiguration config;
  WrenVM *vm;

  wrenInitConfiguration(&config);

  config.writeFn = &wren_onwrite;
  config.errorFn = &wren_onerror;

  vm = wrenNewVM(&config);

  return vm;
}

static void free_vm(WrenVM *vm) { wrenFreeVM(vm); }


static s32 run(void) {
  bool quit = false;
  p8_Event event;
  p8_Canvas *screen, *hero;
  p8_Point points[4] = {{13, 13}, {13, 15}, {15, 13}, {15, 15}};
  p8_Point lines[4] = {{320, 200}, {300, 240}, {340, 240}, {320, 200}};
  p8_Rect rect = {150, 150, 500, 50};
  p8_Rect rects[2] = {{200, 220, 100, 50}, {200, 300, 100, 50}};
  p8_Rect rects1[2] = {{215, 235, 100, 50}, {215, 315, 100, 50}};
  p8_Rect hero_pos, clip_text = {200, 50, 110, 12};
  const char *text = "中英文abc混合ABC测试!";
  const s32 SCREEN_W = 800;
  const s32 SCREEN_H = 600;

  if (!p8_init(SCREEN_W, SCREEN_H, "Think Pixels", 0))
    return -1;

  screen = p8_canvas(SCREEN_W, SCREEN_H);

  if (!screen) {
    p8_quit();
    return -1;
  }

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
    p8_rects(screen, &rect, 1);
    p8_fillrects(screen, rects, 2);

    p8_color(screen, p8_rgba(0, 0xff, 0, 0x5f));
    p8_fillrects(screen, rects1, 2);

    p8_blit(screen, hero, NULL, &hero_pos);

    if (p8_font("Unifont", 16)) {
      p8_text(screen, text, 200, 20, p8_rgb(0xff, 0, 0));

      p8_clip(screen, &clip_text);
      p8_text(screen, text, clip_text.x, clip_text.y, p8_rgb(0, 0xff, 0xff));
      p8_clip(screen, NULL);
    }

    p8_present(screen);
    p8_wait();
  }

  p8_destroy(hero);
  p8_destroy(screen);

  p8_quit();
  return 0;
}

int p8_main(int argc, char *argv[]) {
  const char *module;
  u8 *source;
  WrenVM *vm;
  WrenInterpretResult result;

  if (argc <= 1)
    return show_version();

  module = argv[1];
  source = load_source(module);

  if (!source) {
    error_box("Could not find file \"%s/main.wren\".\n", module);
    return -1;
  }

  vm = init_vm();

  if (!vm)
    return -1;

  result = wrenInterpret(vm, module, source);
  free(source);

  if (result == WREN_RESULT_SUCCESS) {
    run();
  }

  free_vm(vm);

  return 0;
}
