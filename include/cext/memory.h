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

#define X_VOIDPTR_NOTNULL(function, fmt, ...)   \
  {						\
    void *p = function(__VA_ARGS__);		\
    if(p == NULL) xabortf(fmt, __VA_ARGS__);    \
    return p;					\
  }

ATTR_FUN_MALLOC
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(1)
extern void *xmalloc(size_t s);

ATTR_FUN_MALLOC
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(1, 2)
extern void *xcalloc(size_t s, size_t n);

ATTR_ARG_NONNULL(1)
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(2)
extern void *xrealloc(void *ptr, size_t s);

extern void xfree(void *p);

extern void lfree(void *p);

ATTR_ARG_NONNULL(1)
extern void ptrfree(void **p);

extern void xptrfree(void **p);

extern void lptrfree(void **p);

#endif /* !CPLR_MEMORY_H */
