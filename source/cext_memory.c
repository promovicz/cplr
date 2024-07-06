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

CEXT_FUNC_MALLOC
CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_ALLOC_SIZE(1)
void *cext_malloc(size_t s) {
  CEXT_VOIDPTR_NOTNULL(malloc, "Allocation of size %zu failed", s);
}

CEXT_FUNC_MALLOC
CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_ALLOC_SIZE(1, 2)
void *cext_calloc(size_t n, size_t s) {
  CEXT_VOIDPTR_NOTNULL(calloc, "Allocation of %zu objects of size %zu failed", n, s);
}

CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_NONNULL(1)
CEXT_FUNC_ARG_ALLOC_SIZE(2)
void *cext_realloc(void *ptr, size_t s) {
  CEXT_VOIDPTR_NOTNULL(realloc, "Reallocation of %p to size %zu failed", ptr, s);
}

void cext_free(void *p) {
  if(!p) {
    cext_aborts("xfree: Trying to free a NULL pointer");
  }
  free(p);
}

void cext_lfree(void *p) {
  if(p) {
    free(p);
  }
}

CEXT_FUNC_ARG_NONNULL(1)
void cext_ptrfree(void **p) {
  free(*p);
  *p = NULL;
}

void cext_xptrfree(void **p) {
  if(!p) {
    cext_aborts("xptrfree: Trying to free a NULL-referenced pointer");
  }
  if(!*p) {
    cext_aborts("xptrfree: Trying to free a NULL pointer");
  }
  cext_ptrfree(p);
}

void cext_lptrfree(void **p) {
  if(!p) {
    cext_aborts("lptrfree: Trying to free a NULL-referenced pointer");
  }
  if(*p) {
    free(*p);
    *p = NULL;
  }
}
