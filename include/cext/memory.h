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

#ifndef CPLR_MEMORY_H
#define CPLR_MEMORY_H

#include <stdlib.h>

#include "abort.h"
#include "attr.h"

#define CEXT_VOIDPTR_NOTNULL(function, fmt, ...)     \
  {                                                  \
    void *p = function(__VA_ARGS__);                 \
    if(p == NULL) cext_abortf(fmt, __VA_ARGS__);    \
    return p;                                        \
  }

CEXT_FUNC_MALLOC
CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_ALLOC_SIZE(1)
extern void *cext_malloc(size_t s);

CEXT_FUNC_MALLOC
CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_ALLOC_SIZE(1, 2)
extern void *cext_calloc(size_t n, size_t s);

CEXT_FUNC_ARG_NONNULL(1)
CEXT_FUNC_RETURNS_NONNULL
CEXT_FUNC_ARG_ALLOC_SIZE(2)
extern void *cext_realloc(void *ptr, size_t s);

extern void cext_free(void *p);

extern void cext_lfree(void *p);

CEXT_FUNC_ARG_NONNULL(1)
extern void cext_ptrfree(void **p);

extern void cext_xptrfree(void **p);

extern void cext_lptrfree(void **p);

#endif /* !CPLR_MEMORY_H */
