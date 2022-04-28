/*
 * owl_platform.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_PLATFORM_H__
#define __OWL_PLATFORM_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if OWL_WINDOWS
extern u32 owl_peSize(s64 *filesize);
#endif

#ifdef __cplusplus
};
#endif

#endif /* __OWL_PLATFORM_H__ */
