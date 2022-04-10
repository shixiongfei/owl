/*
 * owl_dylib.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#ifndef _WIN32
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#else
#include <Windows.h>
#endif

#include "owl.h"

owl_Dylib owl_dylib(const char *sofile) {
#ifndef _WIN32
  owl_Dylib dylib = dlopen(sofile, RTLD_NOW);
  if (dylib)
    dlclose(dylib);
  return dylib;
#else
  return (owl_Dylib)GetModuleHandleA(sofile);
#endif
}

void *owl_dysym(owl_Dylib dylib, const char *name) {
#ifndef _WIN32
  void *symbol = dlsym(dylib, name);

  if (NULL == symbol) {
    /* append an underscore for platforms that need that. */
    size_t len = strlen(name) + 2;
    char _name[len];

    snprintf(_name, len, "_%s", name);
    symbol = dlsym(dylib, _name);
  }
  return symbol;
#else
  return (void *)GetProcAddress((HMODULE)dylib, name);
#endif
}
