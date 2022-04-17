/*
 * owlvm.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "owlvm.h"

const char *owlvm_version(s32 *major, s32 *minor, s32 *patch) {
  static char owlvmver[] = {OWLVM_RELEASE};

  if (major)
    *major = OWLVM_MAJOR;

  if (minor)
    *minor = OWLVM_MINOR;

  if (patch)
    *patch = OWLVM_PATCH;

  return owlvmver;
}
