#ifndef CPLR_MEMORY_H
#define CPLR_MEMORY_H

#include <stdlib.h>

#include "abort.h"
#include "attr.h"

#define X_VOIDPTR_NOTNULL(function, fmt, ...)   \
  {                                             \
    void *p = function(__VA_ARGS__);            \
    if(p == NULL) xabortf(fmt, __VA_ARGS__);    \
    return p;                                   \
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

ATTR_ARG_NONNULL(1)
extern void xfree(void *p);

ATTR_ARG_NONNULL(1)
extern void ptrfree(void **p);

ATTR_ARG_NONNULL(1)
extern void xptrfree(void **p);

#endif /* !CPLR_MEMORY_H */
