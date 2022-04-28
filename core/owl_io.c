/*
 * owl_io.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>

#if defined(__APPLE__)
/* link CoreFoundation.framework */
#include <CoreFoundation/CoreFoundation.h>
#endif /* __APPLE__ */

#if defined(__linux__)
#include <sys/param.h>
#endif /* __linux__ */

#define OWL_PATHSEP '/'
#else /* _WIN32 */
#include <Windows.h>
#include <io.h>

#define lstat stat

#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif

#define S_ISTYPE(m, TYPE) (TYPE == ((m)&S_IFMT))
#define S_ISDIR(m) S_ISTYPE(m, S_IFDIR)
#define S_ISREG(m) S_ISTYPE(m, S_IFREG)
#define S_ISLNK(m) S_ISTYPE(m, S_IFLNK)

#define access _access

#define OWL_PATHSEP '\\'
#endif /* _WIN32 */

#include "owl_io.h"

#ifndef MAX_PATH
#if defined(MAXPATHLEN)
#define MAX_PATH MAXPATHLEN
#else /* MAXPATHLEN */
#define MAX_PATH 260
#endif /* MAXPATHLEN */
#endif /* MAX_PATH */

#ifndef O_TEMPORARY
#define O_TEMPORARY 0
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define OWL_PATH_MAX_COMPONENTS (MAX_PATH >> 1)

typedef struct owl_PathSlice {
  const char *start;
  const char *end;
} owl_PathSlice;

s64 owl_fileSize(const char *filename) {
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

u8 *owl_readFile(const char *filename) {
  FILE *fp;
  s64 size;
  u8 *data;

  size = owl_fileSize(filename);

  if (size < 0)
    return NULL;

  data = (u8 *)malloc(size + 1);

  if (!data)
    return NULL;

  fp = fopen(filename, "rb");

  if (!fp) {
    free(data);
    return NULL;
  }

  fread(data, size, 1, fp);
  fclose(fp);

  data[size] = 0;
  return data;
}

int owl_tempFile(const char *filename) {
  int fd = -1;

  if (!filename)
    return -1;

  fd = open(filename,
            O_CREAT | O_EXCL | O_TRUNC | O_RDWR | O_BINARY | O_TEMPORARY, 0755);
  if (fd > 0)
    unlink(filename);
  return fd;
}

const char *owl_selfName(void) {
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

OWL_INLINE bool owl_isSeparator(char ch) {
  if (ch == '/')
    return true;

#ifdef _WIN32
  if (ch == '\\')
    return true;
#endif

  return false;
}

char *owl_dirName(char *outbuf, const char *path) {
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
  while (endp > path && owl_isSeparator(*endp))
    endp--;

  /* Find the start of the dir */
  while (endp > path && !owl_isSeparator(*endp))
    endp--;

  /* Either the dir is "/" or there are no slashes */
  if (endp == path) {
    outbuf[0] = owl_isSeparator(*endp) ? OWL_PATHSEP : '.';
    outbuf[1] = '\0';
    return outbuf;
  } else {
    do {
      endp--;
    } while (endp > path && owl_isSeparator(*endp));
  }

  len = (s32)(endp - path + 1);
  strncpy(outbuf, path, len);
  outbuf[len] = '\0';

  return outbuf;
}

char *owl_baseName(char *outbuf, const char *path) {
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
  while (endp > path && owl_isSeparator(*endp))
    endp--;

  /* All slashes becomes "/" */
  if (endp == path && owl_isSeparator(*endp)) {
    outbuf[0] = OWL_PATHSEP;
    outbuf[1] = '\0';
    return outbuf;
  }

  /* Find the start of the base */
  startp = endp;
  while (startp > path && !owl_isSeparator(*(startp - 1)))
    startp--;

  len = (s32)(endp - startp + 1);
  strncpy(outbuf, startp, len);
  outbuf[len] = '\0';

  return outbuf;
}

char *owl_pathFormat(char *path) {
  char *ptr = path;
  s32 i, j = 0;

  while (*ptr) {
    if (owl_isSeparator(*ptr))
      *ptr = OWL_PATHSEP;

    ptr += 1;
    j += 1;
  }

  for (i = 0; i < (j - 1); ++i) {
    if ((OWL_PATHSEP == path[i]) && (path[i] == path[i + 1])) {
      memmove(&path[i], &path[i + 1], j - i);

      i -= 1;
      j -= 1;
    }
  }

  path[j] = 0;
  return path;
}

#ifdef _WIN32
OWL_INLINE bool owl_isDriveLetter(char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
#endif

OWL_INLINE s32 owl_absPrefixLen(const char *path) {
#ifdef _WIN32
  if (owl_isDriveLetter(path[0]) && path[1] == ':')
    return owl_isSeparator(path[2]) ? 3 : 2;
#endif
  return owl_isSeparator(path[0]) ? 1 : 0;
}

s32 owl_pathType(const char *path) {
  if (owl_absPrefixLen(path) > 0)
    return OWL_PATHTYPE_ABSOLUTE;

  if (path[0] == '.' && owl_isSeparator(path[1]))
    return OWL_PATHTYPE_RELATIVE;

  if (path[0] == '.' && path[1] == '.' && owl_isSeparator(path[2]))
    return OWL_PATHTYPE_RELATIVE;

  return OWL_PATHTYPE_SIMPLE;
}

char *owl_resolvePath(char *outbuf, const char *path) {
  owl_PathSlice components[OWL_PATH_MAX_COMPONENTS] = {0};
  const char *startp = path + owl_absPrefixLen(path);
  const char *endp = startp;
  s32 i, len, offset = 0, leading = 0, num_comp = 0;
  bool needsep = false;

  for (;;) {
    if (*endp == '\0' || owl_isSeparator(*endp)) {
      /* Add the current component. */
      if (startp != endp) {
        len = (s32)(endp - startp);

        if (len == 1 && startp[0] == '.') {
          /* Skip "." components. */
        } else if (len == 2 && startp[0] == '.' && startp[1] == '.') {
          /* Walk out of directories on "..". */
          if (num_comp > 0)
            num_comp--; /* Discard the previous component. */
          else
            leading++; /* Can't back out any further, so preserve the "..". */
        } else {
          /* Cannot have any more path components */
          if (num_comp > OWL_PATH_MAX_COMPONENTS)
            return NULL;

          components[num_comp].start = startp;
          components[num_comp].end = endp;
          num_comp++;
        }
      }

      /* Skip over separators. */
      while (*endp != '\0' && owl_isSeparator(*endp))
        endp++;

      startp = endp;

      if (*endp == '\0')
        break;
    }

    endp++;
  }

  len = owl_absPrefixLen(path);
  if (len > 0) {
    /* It's an absolute path, so preserve the absolute prefix. */
    strncpy(outbuf + offset, path, len);
    offset += len;

    /* Fix the separator */
    if (owl_isSeparator(outbuf[offset - 1]))
      outbuf[offset - 1] = OWL_PATHSEP;
  } else if (leading > 0) {
    /* Add any leading "..". */
    for (i = 0; i < leading; ++i) {
      if (needsep)
        outbuf[offset++] = OWL_PATHSEP;

      outbuf[offset++] = '.';
      outbuf[offset++] = '.';

      needsep = true;
    }
  } else if (path[0] == '.' && owl_isSeparator(path[1])) {
    /* Preserve a leading "./" */
    outbuf[offset++] = '.';
    needsep = true;
  }

  for (i = 0; i < num_comp; ++i) {
    len = (s32)(components[i].end - components[i].start);

    if (needsep)
      outbuf[offset++] = OWL_PATHSEP;

    strncpy(outbuf + offset, components[i].start, len);
    offset += len;

    needsep = true;
  }

  if (offset == 0)
    outbuf[offset++] = '.';

  outbuf[offset] = '\0';
  return outbuf;
}

void owl_setWorkDir(const char *workdir) {
#ifdef _WIN32
  SetCurrentDirectoryA(workdir);
#else
  chdir(workdir);
#endif
}

char *owl_getWorkDir(char *workdir, s32 size) {
#ifdef _WIN32
  GetCurrentDirectoryA(size, workdir);
#else
  getcwd(workdir, size);
#endif
  return workdir;
}

bool owl_isExist(const char *path) { return 0 == access(path, 0); }

bool owl_isDir(const char *path) {
  struct stat st;
  return 0 == lstat(path, &st) ? !!S_ISDIR(st.st_mode) : 0;
}

bool owl_isFile(const char *path) {
  struct stat st;
  return 0 == lstat(path, &st) ? !!S_ISREG(st.st_mode) : 0;
}

bool owl_isLink(const char *path) {
  struct stat st;
  return 0 == lstat(path, &st) ? !!S_ISLNK(st.st_mode) : 0;
}
