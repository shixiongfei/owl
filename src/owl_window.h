/*
 * owl_window.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_WINDOW_H__
#define __OWL_WINDOW_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct owl_Window owl_Window;

extern owl_Window *owl_window(s32 w, s32 h, const char *title, s32 flags);
extern void owl_freewindow(owl_Window *window);
extern bool owl_windowevent(owl_Window *window, owl_Event *event);
extern bool owl_windowtextinputshown(owl_Window *window);
extern void owl_windowtextinputposition(owl_Window *window, s32 x, s32 y);
extern owl_Canvas *owl_windowcanvas(owl_Window *window);
extern void owl_windowpresent(owl_Window *window);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_WINDOW_H__ */
