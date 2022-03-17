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

#include <stdio.h>
#include <string.h>

#include "wren.h"

#include "p8.h"

static void wren_onwrite(WrenVM *vm, const char *text) { printf("%s", text); }

static void wren_onerror(WrenVM *vm, WrenErrorType type, const char *module,
                         const int line, const char *msg) {
  switch (type) {
  case WREN_ERROR_COMPILE:
    printf("[%s line %d] [Error] %s\n", module, line, msg);
    break;
  case WREN_ERROR_STACK_TRACE:
    printf("[%s line %d] in %s\n", module, line, msg);
    break;
  case WREN_ERROR_RUNTIME:
    printf("[Runtime Error] %s\n", msg);
    break;
  }
}

static s32 p8_run(s32 argc, char *argv[]) {
  WrenConfiguration config;
  WrenVM *vm;

  wrenInitConfiguration(&config);

  config.writeFn = &wren_onwrite;
  config.errorFn = &wren_onerror;

  vm = wrenNewVM(&config);

  if (!vm)
    return -1;


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

  p8_loadfont("宋体", "./simsun.ttc");

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

    if (p8_font("宋体", 18)) {
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


  wrenFreeVM(vm);

  return 0;
}

static s32 p8_showversion(void) {
  p8_MsgBoxButton mbtn = {P8_MSGBOX_RETURNKEY | P8_MSGBOX_ESCKEY, 0, "OK"};
  char title[] = {"P8 Game Engine"};
  char version[] = {"P8 v" P8_RELEASE};
  char build[] = {"Build: " __DATE__ " " __TIME__};
  char author[] = {"By: " P8_AUTHOR};
  char message[sizeof(version) + sizeof(build) + sizeof(author) + 1] = {0};

  sprintf(message, "%s\n%s\n\n%s", version, build, author);
  p8_msgbox(P8_MSGBOX_INFORMATION, title, message, &mbtn, 1);

  return 0;
}

int p8_main(int argc, char *argv[]) {
  return argc > 1 ? p8_run(argc, argv) : p8_showversion();
}
