/*
 * owl_io.h
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef __OWL_IO_H__
#define __OWL_IO_H__

#include "owl.h"

#define OWL_PATHTYPE_ABSOLUTE 0
#define OWL_PATHTYPE_RELATIVE 1
#define OWL_PATHTYPE_SIMPLE 2

#ifdef __cplusplus
extern "C" {
#endif

extern s64 owl_filesize(const char *filename);
extern u8 *owl_readfile(const char *filename);
extern int owl_tempfile(const char *filename);

extern const char *owl_selfname(void);
extern char *owl_pathformat(char *path);
extern char *owl_dirname(char *outbuf, const char *path);
extern char *owl_basename(char *outbuf, const char *path);

extern s32 owl_pathtype(const char *path);
extern char *owl_resolvepath(char *outbuf, const char *path);

extern void owl_setcwd(const char *workdir);
extern char *owl_getcwd(char *workdir, s32 size);

extern bool owl_isexist(const char *path);
extern bool owl_isdir(const char *path);
extern bool owl_isfile(const char *path);
extern bool owl_islink(const char *path);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_IO_H__ */
