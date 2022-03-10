/*
 * p8_io.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include "p8.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

s64 p8_filesize(const char *filename) {
  s64 size;
  int fp;

  fp = open(filename, O_RDONLY | O_BINARY);

  if (fp < 0)
    return -1;

  lseek(fp, 0, SEEK_END);
  size = lseek(fp, 0, SEEK_CUR);
  close(fp);

  return size;
}

u8 *p8_readfile(const char *filename) {
  FILE *fp;
  s64 size;
  u8 *data;

  size = p8_filesize(filename);

  if (size < 0)
    return NULL;

  data = (u8 *)malloc(size);

  if (!data)
    return NULL;

  fp = fopen(filename, "rb");

  if (!fp) {
    free(data);
    return NULL;
  }

  fread(data, size, 1, fp);
  fclose(fp);

  return data;
}
