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

f64 owl_degrees(f64 rad) { return rad * OWL_DEG; }

f64 owl_radians(f64 deg) { return deg * OWL_RAD; }

void owl_matrix(owl_Matrix *m, f64 a, f64 b, f64 c, f64 d, f64 tx, f64 ty) {
  m->a = a, m->c = c, m->tx = tx;
  m->b = b, m->d = d, m->ty = ty;
}

void owl_matrixSetIdentity(owl_Matrix *m) {
  m->a = 1.0, m->c = 0.0, m->tx = 0.0;
  m->b = 0.0, m->d = 1.0, m->ty = 0.0;
}

void owl_matrixSetTranslate(owl_Matrix *m, f64 x, f64 y) {
  owl_matrix(m, 1.0, 0.0, 0.0, 1.0, x, y);
}

void owl_matrixSetScale(owl_Matrix *m, f64 x, f64 y) {
  owl_matrix(m, x, 0.0, 0.0, y, 0.0, 0.0);
}

void owl_matrixSetShear(owl_Matrix *m, f64 x, f64 y) {
  owl_matrix(m, 1.0, tan(y), tan(x), 1.0, 0.0, 0.0);
}

void owl_matrixSetRotate(owl_Matrix *m, f64 rad) {
  f64 c = cos(rad);
  f64 s = sin(rad);

  owl_matrix(m, c, s, -s, c, 0.0, 0.0);
}

void owl_matrixSetTransRotate(owl_Matrix *m, f64 x, f64 y, f64 rad) {
  f64 c = cos(rad);
  f64 s = sin(rad);

  f64 cx = x * (1.0 - c) + y * s;
  f64 cy = y * (1.0 - c) - x * s;

  owl_matrix(m, c, s, -s, c, cx, cy);
}

void owl_matrixMultiply(owl_Matrix *m, owl_Matrix *l, owl_Matrix *r) {
  f64 a = l->a * r->a + l->c * r->b;
  f64 b = l->b * r->a + l->d * r->b;
  f64 c = l->a * r->c + l->c * r->d;
  f64 d = l->b * r->c + l->d * r->d;

  f64 tx = l->a * r->tx + l->c * r->ty + l->tx;
  f64 ty = l->b * r->tx + l->d * r->ty + l->ty;

  owl_matrix(m, a, b, c, d, tx, ty);
}

bool owl_matrixInvert(owl_Matrix *m) {
  f64 a, b, c, d, tx, ty, invdet;
  f64 det = m->a * m->d - m->b * m->c;

  if (fabs(det - 0.0) < DBL_EPSILON)
    return false;

  invdet = 1.0 / det;

  a = invdet * m->d;
  b = -invdet * m->b;
  c = -invdet * m->c;
  d = invdet * m->a;

  tx = (m->c * m->ty - m->d * m->tx) * invdet;
  ty = (m->b * m->tx - m->a * m->ty) * invdet;

  owl_matrix(m, a, b, c, d, tx, ty);
  return true;
}

void owl_matrixTranslate(owl_Matrix *m, f64 x, f64 y) {
  owl_Matrix mat;
  owl_matrixSetTranslate(&mat, x, y);
  owl_matrixMultiply(m, m, &mat);
}

void owl_matrixScale(owl_Matrix *m, f64 x, f64 y) {
  owl_Matrix mat;
  owl_matrixSetScale(&mat, x, y);
  owl_matrixMultiply(m, m, &mat);
}

void owl_matrixShear(owl_Matrix *m, f64 x, f64 y) {
  owl_Matrix mat;
  owl_matrixSetShear(&mat, x, y);
  owl_matrixMultiply(m, m, &mat);
}

void owl_matrixRotate(owl_Matrix *m, f64 rad) {
  owl_Matrix mat;
  owl_matrixSetRotate(&mat, rad);
  owl_matrixMultiply(m, m, &mat);
}

void owl_matrixTransRotate(owl_Matrix *m, f64 x, f64 y, f64 rad) {
  owl_Matrix mat;
  owl_matrixSetTransRotate(&mat, x, y, rad);
  owl_matrixMultiply(m, m, &mat);
}

void owl_matrixApply(owl_Matrix *m, owl_Vector2 *v, f64 x, f64 y) {
  v->x = (m->a * x) + (m->c * y) + m->tx;
  v->y = (m->b * x) + (m->d * y) + m->ty;
}
