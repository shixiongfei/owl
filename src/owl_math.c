/*
 * owl_math.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include <float.h>
#include <math.h>

#include "owl_math.h"

f32 owl_degrees(f32 rad) { return rad * OWL_DEG; }

f32 owl_radians(f32 deg) { return deg * OWL_RAD; }

void owl_matrix(owl_Matrix *m, f32 a, f32 b, f32 c, f32 d, f32 tx, f32 ty) {
  m->a = a, m->c = c, m->tx = tx;
  m->b = b, m->d = d, m->ty = ty;
}

void owl_matrix_setidentity(owl_Matrix *m) {
  m->a = 1.0f, m->c = 0.0f, m->tx = 0.0f;
  m->b = 0.0f, m->d = 1.0f, m->ty = 0.0f;
}

void owl_matrix_settranslate(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, 1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

void owl_matrix_setscale(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, x, 0.0f, 0.0f, y, 0.0f, 0.0f);
}

void owl_matrix_setshear(owl_Matrix *m, f32 x, f32 y) {
  owl_matrix(m, 1.0f, tanf(y), tanf(x), 1.0f, 0.0f, 0.0f);
}

void owl_matrix_setrotate(owl_Matrix *m, f32 rad) {
  f32 c = cosf(rad);
  f32 s = sinf(rad);

  owl_matrix(m, c, s, -s, c, 0.0f, 0.0f);
}

void owl_matrix_settransrotate(owl_Matrix *m, f32 x, f32 y, f32 rad) {
  f32 c = cosf(rad);
  f32 s = sinf(rad);

  f32 cx = x * (1.0f - c) + y * s;
  f32 cy = y * (1.0f - c) - x * s;

  owl_matrix(m, c, s, -s, c, cx, cy);
}

void owl_matrix_multiply(owl_Matrix *m, owl_Matrix *l, owl_Matrix *r) {
  f32 a = l->a * r->a + l->c * r->b;
  f32 b = l->b * r->a + l->d * r->b;
  f32 c = l->a * r->c + l->c * r->d;
  f32 d = l->b * r->c + l->d * r->d;

  f32 tx = l->a * r->tx + l->c * r->ty + l->tx;
  f32 ty = l->b * r->tx + l->d * r->ty + l->ty;

  owl_matrix(m, a, b, c, d, tx, ty);
}

bool owl_matrix_invert(owl_Matrix *m) {
  f32 a, b, c, d, tx, ty, invdet;
  f32 det = m->a * m->d - m->b * m->c;

  if (fabs(det - 0.0f) < FLT_EPSILON)
    return false;

  invdet = 1.0f / det;

  a = invdet * m->d;
  b = -invdet * m->b;
  c = -invdet * m->c;
  d = invdet * m->a;

  tx = (m->c * m->ty - m->d * m->tx) * invdet;
  ty = (m->b * m->tx - m->a * m->ty) * invdet;

  owl_matrix(m, a, b, c, d, tx, ty);
  return true;
}

void owl_matrix_translate(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_settranslate(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_scale(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_setscale(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_shear(owl_Matrix *m, f32 x, f32 y) {
  owl_Matrix mat;
  owl_matrix_setshear(&mat, x, y);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_rotate(owl_Matrix *m, f32 rad) {
  owl_Matrix mat;
  owl_matrix_setrotate(&mat, rad);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_transrotate(owl_Matrix *m, f32 x, f32 y, f32 rad) {
  owl_Matrix mat;
  owl_matrix_settransrotate(&mat, x, y, rad);
  owl_matrix_multiply(m, m, &mat);
}

void owl_matrix_apply(owl_Matrix *m, owl_Vector2 *v, f32 x, f32 y) {
  v->x = (m->a * x) + (m->c * y) + m->tx;
  v->y = (m->b * x) + (m->d * y) + m->ty;
}
