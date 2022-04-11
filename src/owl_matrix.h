/*
 * owl_matrix.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_MATRIX_H__
#define __OWL_MATRIX_H__

#include "owl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct owl_Matrix {
  f32 a, c, tx;
  f32 b, d, ty;
  /*  0, 0, 1 */
} owl_Matrix;

#ifdef __cplusplus
};
#endif

#endif /* __OWL_MATRIX_H__ */
