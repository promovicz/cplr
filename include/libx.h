#ifndef LIBX_H
#define LIBX_H

#include <stdio.h>
#include <stdlib.h>

#include "attr.h"

#define X_VOIDPTR_NOTNULL(function, fmt, ...)   \
  {                                             \
    void *p = function(__VA_ARGS__);            \
    if(p == NULL) xabortf(fmt, __VA_ARGS__);    \
    return p;                                   \
  }

ATTR_FUN_COLD
ATTR_FUN_NORETURN
void xabortm(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  abort();
}

ATTR_FUN_NORETURN
void xabortf(const char *fmt, ...) {
  va_list a;
  va_start(a, fmt);
  vfprintf(stderr, fmt, a);
  va_end(a);
  fputc('\n', stderr);
  abort();
}

ATTR_FUN_MALLOC
ATTR_ARG_ALLOC_SIZE(1)
void *xmalloc(size_t s) {
  X_VOIDPTR_NOTNULL(malloc, "Allocation of size %zu failed", s);
}

ATTR_FUN_MALLOC
ATTR_ARG_ALLOC_SIZE(1, 2)
void *xcalloc(size_t s, size_t n) {
  X_VOIDPTR_NOTNULL(calloc, "Allocation of %zu objects of size %zu failed", s, n);
}

ATTR_ARG_NONNULL(1)
ATTR_ARG_ALLOC_SIZE(2)
void *xrealloc(void *ptr, size_t s) {
  X_VOIDPTR_NOTNULL(realloc, "Reallocation of %p to size %zu failed", ptr, s);
}

ATTR_ARG_NONNULL(1)
void xfree(void *p) {
  if(!p) {
    xabortm("xfree: Trying to free a NULL pointer");
  }
  free(p);
}

ATTR_ARG_NONNULL(1)
void ptrfree(void **p) {
  free(*p);
  *p = NULL;
}

ATTR_ARG_NONNULL(1)
void xptrfree(void **p) {
  if(!(p && *p)) {
    xabortm("xptrfree: Trying to free a NULL pointer");
  }
  ptrfree(p);
}

#endif /* !LIBX_H */
