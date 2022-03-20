/*
 * p8_windows.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of P8.
 * Usage of P8 is subject to the appropriate license agreement.
 */

#if _WIN32
#include <Windows.h>
#endif

#include "p8.h"

#if _WIN32
/* @copy /b src.exe+your.data dst.exe */
u32 p8_pesize(s64 *filesize) {
  LARGE_INTEGER liFileSize = {0};
  DWORD dwPESize = 0;
  DWORD dwRead, dwMaxPointer = 0;
  IMAGE_DOS_HEADER *pDOSHeader;
  IMAGE_NT_HEADERS *pHeader;
  IMAGE_SECTION_HEADER *pSectionTable;
  BYTE pBuff[4096] = {0};
  WORD i;

  HANDLE hFile = CreateFileA(p8_selfname(), GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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
#endif
