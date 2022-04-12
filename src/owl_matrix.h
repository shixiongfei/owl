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

typedef struct owl_Vector2 {
  f32 x, y;
} owl_Vector2;

typedef struct owl_Matrix {
  f32 a, c, tx;
  f32 b, d, ty;
  /*  0, 0, 1 */
} owl_Matrix;

OWL_INLINE void owl_matrix(owl_Matrix *m, f32 a, f32 b, f32 c, f32 d, f32 tx,
                           f32 ty) {
  m->a = a, m->c = c;
  m->b = b, m->d = d;
  m->tx = tx, m->ty = ty;
}

OWL_INLINE void owl_matrix_setidentity(owl_Matrix *m) {
  m->a = 1.0f, m->c = 0.0f;
  m->b = 0.0f, m->d = 1.0f;
  m->tx = 0.0f, m->ty = 0.0f;
}

OWL_INLINE void owl_matrix_settranslate(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, 1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

OWL_INLINE void owl_matrix_setscale(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, x, 0.0f, 0.0f, y, 0.0f, 0.0f);
}

extern void owl_matrix_setshear(owl_Matrix *m, f32 x, f32 y);
extern void owl_matrix_setrotate(owl_Matrix *m, f32 radians);
extern void owl_matrix_settransrotate(owl_Matrix *m, f32 x, f32 y, f32 radians);

extern void owl_matrix_multiply(owl_Matrix *m, owl_Matrix *l, owl_Matrix *r);
extern bool owl_matrix_invert(owl_Matrix *m);

extern void owl_matrix_translate(owl_Matrix *m, f32 x, f32 y);
extern void owl_matrix_scale(owl_Matrix *m, f32 x, f32 y);
extern void owl_matrix_shear(owl_Matrix *m, f32 x, f32 y);
extern void owl_matrix_rotate(owl_Matrix *m, f32 radians);
extern void owl_matrix_transrotate(owl_Matrix *m, f32 x, f32 y, f32 radians);
extern void owl_matrix_apply(owl_Matrix *m, owl_Vector2 *out, f32 x, f32 y);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_MATRIX_H__ */
