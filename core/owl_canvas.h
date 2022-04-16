/*
 * owl_canvas.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_CANVAS_H__
#define __OWL_CANVAS_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void owl_lockcanvas(owl_Canvas *canvas);
extern void owl_unlockcanvas(owl_Canvas *canvas);

extern void owl_drawpixel(owl_Canvas *canvas, s32 x, s32 y, u32 color);
extern void owl_drawline(owl_Canvas *canvas, s32 x1, s32 y1, s32 x2, s32 y2,
                         u32 color);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_CANVAS_H__ */
