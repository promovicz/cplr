/*
 * cplr - Utility for running C code
 *
 * Copyright (C) 2020-2021 Ingo Albrecht <copyright@promovicz.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "cext/string.h"

char *xstrdup(char *s) {
  char *res = NULL;

  if(s) {
    res = strdup(s);
  }

  return res;
}

bool strprefix(const char *str, const char *pfx) {
  return strncmp(str, pfx, strlen(pfx)) == 0;
}

bool strsuffix(const char *str, const char *suf)
{
  if (!str || !suf)
    return false;
  size_t lenstr = strlen(str);
  size_t lensuf = strlen(suf);
  if (lensuf >  lenstr)
    return false;
  return strncmp(str + lenstr - lensuf, suf, lensuf) == 0;
}

ATTR_FUN_FORMAT(printf, 2, 0)
ATTR_ARG_FORMAT(2)
char *vmsnprintf(size_t limit, const char *fmt, va_list a) {
  int len, chk;
  char *buf;
  va_list x;
  // clamp limit
  if(limit > INT_MAX) {
    limit = INT_MAX;
  }
  // determine length
  va_copy(x, a);
  len = vsnprintf(NULL, 0, fmt, x);
  va_end(x);
  if(len > (int)limit) {
    return NULL;
  }
  // allocate
  buf = malloc(len + 1);
  if(!buf) {
    return NULL;
  }
  // format
  va_copy(x, a);
  chk = vsnprintf(buf, len + 1, fmt, x);
  va_end(x);
  // verify
  assert(chk == len);
  // done
  return buf;
}

ATTR_FUN_FORMAT(printf, 2, 3)
ATTR_ARG_FORMAT(2)
char *msnprintf(size_t limit, const char *fmt, ...) {
  char *res;
  va_list a;
  va_start(a, fmt);
  res = vmsnprintf(limit, fmt, a);
  va_end(a);
  return res;
}

ATTR_FUN_FORMAT(printf, 1, 0)
ATTR_ARG_FORMAT(1)
char *vmsprintf(const char *fmt, va_list a) {
  return vmsnprintf(INT_MAX, fmt, a);
}

ATTR_FUN_FORMAT(printf, 1, 2)
ATTR_ARG_FORMAT(1)
char *msprintf(const char *fmt, ...) {
  char *res;
  va_list a;
  va_start(a, fmt);
  res = vmsnprintf(INT_MAX, fmt, a);
  va_end(a);
  return res;
}
