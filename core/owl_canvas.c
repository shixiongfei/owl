/*
 * owl_canvas.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "owl_canvas.h"

owl_Canvas *owl_load(const char *filename) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

owl_Canvas *owl_loadex(const char *filename, owl_Pixel colorkey) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = (format == STBI_rgb) ? owl_imagex(data, w, h, colorkey)
                                : owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}
