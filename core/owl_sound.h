/*
 * owl_sound.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_SOUND_H__
#define __OWL_SOUND_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool owl_soundInit(void);
extern void owl_soundQuit(void);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_SOUND_H__ */
