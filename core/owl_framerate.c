/*
 * owl_framerate.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "owl_framerate.h"

#define OWL_FRAMERATE_UPPER_LIMIT 100
#define OWL_FRAMERATE_LOWER_LIMIT 1

bool owl_frameRateSet(owl_FrameRate *fr, u32 rate) {
  if (rate < OWL_FRAMERATE_LOWER_LIMIT || rate > OWL_FRAMERATE_UPPER_LIMIT)
    return false;

  fr->framecount = 0;
  fr->rate = rate;
  fr->rateticks = 1000.0f / (f32)rate;
  fr->baseticks = owl_ticks();
  fr->lastticks = fr->baseticks;

  return true;
}

u32 owl_frameRateWait(owl_FrameRate *fr) {
  u64 current_ticks;
  u64 target_ticks;
  u32 time_passed;

  fr->framecount += 1;

  current_ticks = owl_ticks();
  time_passed = (u32)(current_ticks - fr->lastticks);

  fr->lastticks = current_ticks;

  target_ticks = fr->baseticks + (u64)((f32)fr->framecount * fr->rateticks);

  if (current_ticks <= target_ticks)
    owl_sleep((u32)(target_ticks - current_ticks));
  else {
    fr->framecount = 0;
    fr->baseticks = owl_ticks();
  }

  return time_passed;
}
