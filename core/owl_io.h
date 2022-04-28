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

extern s64 owl_fileSize(const char *filename);
extern u8 *owl_readFile(const char *filename);
extern int owl_tempFile(const char *filename);

extern const char *owl_selfName(void);
extern char *owl_dirName(char *outbuf, const char *path);
extern char *owl_baseName(char *outbuf, const char *path);

extern char *owl_pathFormat(char *path);
extern s32 owl_pathType(const char *path);
extern char *owl_resolvePath(char *outbuf, const char *path);

extern void owl_setWorkDir(const char *workdir);
extern char *owl_getWorkDir(char *workdir, s32 size);

extern bool owl_isExist(const char *path);
extern bool owl_isDir(const char *path);
extern bool owl_isFile(const char *path);
extern bool owl_isLink(const char *path);

#ifdef __cplusplus
};
#endif

#endif /* __OWL_IO_H__ */
