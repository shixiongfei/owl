/*
 * owl_platform.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "owl_platform.h"

#if OWL_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */

#include <Windows.h>
#include <time.h>     /* _tzset */

#include "owl_io.h"

typedef long suseconds_t;

struct timeval {
  time_t tv_sec;       /* seconds */
  suseconds_t tv_usec; /* and microseconds */
};

struct timezone {
  int tz_minuteswest; /* minutes W of Greenwich */
  int tz_dsttime;     /* type of dst correction */
};

static int gettimeofday(struct timeval *tv, struct timezone *tz) {
  static const u64 EPOCH = 116444736000000000ULL;
  static int tzflag = 0;
  FILETIME ft;
  ULARGE_INTEGER ularge;
  u64 ms;

  GetSystemTimeAsFileTime(&ft);

  ularge.u.LowPart = ft.dwLowDateTime;
  ularge.u.HighPart = ft.dwHighDateTime;

  /* convert into microseconds */
  ms = (ularge.QuadPart - EPOCH) / 10;

  tv->tv_sec = (time_t)(ms / 1000000);
  tv->tv_usec = (suseconds_t)(ms % 1000000);

  if (tz) {
    if (!tzflag) {
      _tzset();
      tzflag = 1;
    }

    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

/* @copy /b src.exe+your.data dst.exe */
u32 owl_pesize(s64 *filesize) {
  LARGE_INTEGER liFileSize = {0};
  DWORD dwPESize = 0;
  DWORD dwRead, dwMaxPointer = 0;
  IMAGE_DOS_HEADER *pDOSHeader;
  IMAGE_NT_HEADERS *pHeader;
  IMAGE_SECTION_HEADER *pSectionTable;
  BYTE pBuff[4096] = {0};
  WORD i;

  HANDLE hFile = CreateFileA(owl_selfname(), GENERIC_READ, FILE_SHARE_READ,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (INVALID_HANDLE_VALUE == hFile)
    return 0;

  if (!ReadFile(hFile, pBuff, sizeof(pBuff), &dwRead, NULL)) {
    CloseHandle(hFile);
    return 0;
  }

  GetFileSizeEx(hFile, &liFileSize);
  CloseHandle(hFile);

  pDOSHeader = (IMAGE_DOS_HEADER *)pBuff;

  if (IMAGE_DOS_SIGNATURE != pDOSHeader->e_magic)
    return 0;

  if ((ULONG)(pDOSHeader->e_lfanew) >=
      (ULONG)(sizeof(pBuff) - sizeof(IMAGE_NT_HEADERS)))
    return 0;

  /* Locate PE header */
  pHeader = (IMAGE_NT_HEADERS *)(pBuff + pDOSHeader->e_lfanew);

  if (IMAGE_NT_SIGNATURE != pHeader->Signature)
    return 0;

  pSectionTable =
      (IMAGE_SECTION_HEADER *)((BYTE *)pHeader + sizeof(IMAGE_NT_HEADERS));

  if ((BYTE *)pSectionTable >= (pBuff + sizeof(pBuff)))
    return 0;

  /* For each section */
  for (i = 0; i < pHeader->FileHeader.NumberOfSections; ++i, pSectionTable++) {
    if (pSectionTable->PointerToRawData > dwMaxPointer) {
      dwMaxPointer = pSectionTable->PointerToRawData;
      dwPESize = pSectionTable->PointerToRawData + pSectionTable->SizeOfRawData;
    }
  }

  if (filesize)
    *filesize = liFileSize.QuadPart;

  return dwPESize;
}
#else
#include <sys/time.h> /* gettimeofday */
#endif

f64 owl_time(u64 *sec, u32 *usec) {
  struct timeval time;

  gettimeofday(&time, NULL);

  if (sec)
    *sec = (u64)time.tv_sec;

  if (usec)
    *usec = (u32)time.tv_usec;

  return time.tv_sec + time.tv_usec / 1000000.0;
}
