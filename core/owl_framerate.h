/*
 * owl_framerate.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_FRAMERATE_H__
#define __OWL_FRAMERATE_H__

#include "owl.h"

#define OWL_FRAMERATE_DEFAULT 60

#ifdef __cplusplus
extern "C" {
#endif

typedef struct owl_FrameRate {
  u32 rate;
  u32 framecount;
  f32 rateticks;
  u64 baseticks;
  u64 lastticks;
} owl_FrameRate;

extern bool owl_frameRateSet(owl_FrameRate *fr, u32 rate);
extern u32 owl_frameRateWait(owl_FrameRate *fr);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_FRAMERATE_H__ */
