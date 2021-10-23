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

#include "cext/memory.h"

ATTR_FUN_MALLOC
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(1)
void *xmalloc(size_t s) {
  X_VOIDPTR_NOTNULL(malloc, "Allocation of size %zu failed", s);
}

ATTR_FUN_MALLOC
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(1, 2)
void *xcalloc(size_t n, size_t s) {
  X_VOIDPTR_NOTNULL(calloc, "Allocation of %zu objects of size %zu failed", n, s);
}

ATTR_ARG_NONNULL(1)
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(2)
void *xrealloc(void *ptr, size_t s) {
  X_VOIDPTR_NOTNULL(realloc, "Reallocation of %p to size %zu failed", ptr, s);
}

void xfree(void *p) {
  if(!p) {
    xaborts("xfree: Trying to free a NULL pointer");
  }
  free(p);
}

void lfree(void *p) {
  if(p) {
    free(p);
  }
}

ATTR_ARG_NONNULL(1)
void ptrfree(void **p) {
  free(*p);
  *p = NULL;
}

void xptrfree(void **p) {
  if(!p) {
    xaborts("xptrfree: Trying to free a NULL-referenced pointer");
  }
  if(!*p) {
    xaborts("xptrfree: Trying to free a NULL pointer");
  }
  ptrfree(p);
}

void lptrfree(void **p) {
  if(!p) {
    xaborts("lptrfree: Trying to free a NULL-referenced pointer");
  }
  if(*p) {
    free(*p);
    *p = NULL;
  }
}
