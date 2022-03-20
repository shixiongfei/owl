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
#include <string.h>

#ifndef _WIN32
#include <unistd.h>

#if defined(__APPLE__)
/* link CoreFoundation.framework */
#include <CoreFoundation/CoreFoundation.h>
#endif /* __APPLE__ */
#else  /* _WIN32 */
#include <Windows.h>
#include <io.h>
#endif /* _WIN32 */

#include "p8.h"

#ifndef MAX_PATH
#if defined(MAXPATHLEN)
#define MAX_PATH MAXPATHLEN
#else /* MAXPATHLEN */
#define MAX_PATH 256
#endif /* MAXPATHLEN */
#endif /* MAX_PATH */

#ifndef O_TEMPORARY
#define O_TEMPORARY 0
#endif

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

int p8_tempfile(const char *filename) {
  int fd = -1;

  if (!filename)
    return -1;

  fd = open(filename,
            O_CREAT | O_EXCL | O_TRUNC | O_RDWR | O_BINARY | O_TEMPORARY, 0755);
  if (fd > 0)
    unlink(filename);
  return fd;
}

const char *p8_selfname(void) {
  static char buffer[MAX_PATH] = {0};

  if (!buffer[0]) {
#if defined(_WIN32)
    DWORD len = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    buffer[len] = '\0';
#elif defined(__APPLE__)
    CFURLRef exeURL = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
    if (exeURL) {
      CFURLGetFileSystemRepresentation(exeURL, true, (u8 *)buffer,
                                       sizeof(buffer));
      CFRelease(exeURL);
    }
    if (!buffer[0]) {
      u32 nSize = sizeof(buffer);
      _NSGetExecutablePath(buffer, &nSize);
    }
#elif defined(__linux__)
    if (readlink("/proc/self/exe", buffer, sizeof(buffer)) <= 0) {
      char link_name[MAXPATHLEN] = {0};
      sprintf(link_name, "/proc/%d/exe", getpid());
      readlink(link_name, buffer, sizeof(buffer));
    }
#endif
  }
  return buffer;
}

char *p8_pathformat(char *path, char pathsep) {
  char *ptr = path;
  s32 i, j = 0;

  while (*ptr) {
    if (('/' == *ptr) || ('\\' == *ptr))
      *ptr = pathsep;

    ptr += 1;
    j += 1;
  }

  for (i = 0; i < (j - 1); ++i) {
    if ((pathsep == path[i]) && (path[i] == path[i + 1])) {
      memmove(&path[i], &path[i + 1], j - i);

      i -= 1;
      j -= 1;
    }
  }

  path[j] = 0;
  return path;
}

char *p8_dirname(char *outbuf, const char *path, char pathsep) {
  const char *endp;
  s32 len;

  /* Empty or NULL string gets treated as "." */
  if (path == NULL || *path == '\0') {
    outbuf[0] = '.';
    outbuf[1] = '\0';
    return outbuf;
  }

  /* Strip trailing slashes */
  endp = path + strlen(path) - 1;
  while (endp > path && *endp == pathsep)
    endp--;

  /* Find the start of the dir */
  while (endp > path && *endp != pathsep)
    endp--;

  /* Either the dir is "/" or there are no slashes */
  if (endp == path) {
    outbuf[0] = (*endp == pathsep) ? pathsep : '.';
    outbuf[1] = '\0';
    return outbuf;
  } else {
    do {
      endp--;
    } while (endp > path && *endp == pathsep);
  }

  len = (s32)(endp - path + 1);
  strncpy(outbuf, path, len);
  outbuf[len] = '\0';

  return outbuf;
}

char *p8_basename(char *outbuf, const char *path, char pathsep) {
  const char *endp, *startp;
  s32 len;

  /* Empty or NULL string gets treated as "." */
  if (path == NULL || *path == '\0') {
    outbuf[0] = '.';
    outbuf[1] = '\0';
    return outbuf;
  }

  /* Strip trailing slashes */
  endp = path + strlen(path) - 1;
  while (endp > path && *endp == pathsep)
    endp--;

  /* All slashes becomes "/" */
  if (endp == path && *endp == pathsep) {
    outbuf[0] = pathsep;
    outbuf[1] = '\0';
    return outbuf;
  }

  /* Find the start of the base */
  startp = endp;
  while (startp > path && *(startp - 1) != pathsep)
    startp--;

  len = (s32)(endp - startp + 1);
  strncpy(outbuf, startp, len);
  outbuf[len] = '\0';

  return outbuf;
}
