#ifndef LIBX_H
#define LIBX_H

#include <stdio.h>
#include <stdlib.h>

#define ATTR_FUN_ARTIFICIAL                     \
  __attribute__((artificial))
#define ATTR_FUN_COLD                           \
  __attribute__((cold))
#define ATTR_FUN_CONST                          \
  __attribute__((const))
#define ATTR_FUN_PURE                           \
  __attribute__((pure))
#define ATTR_FUN_MALLOC                         \
  __attribute__((malloc))
#define ATTR_FUN_NORETURN                       \
  __attribute__((noreturn))
#define ATTR_FUN_FORMAT(arch, fmtindex, argindex)       \
  __attribute__((format(arch, fmtindex, argindex)))
#define ATTR_ARG_ALLOC_SIZE(...)                \
  __attribute__((alloc_size(__VA_ARGS__)))
#define ATTR_ARG_FORMAT(...)                    \
  __attribute__((format_arg(__VA_ARGS__)))
#define ATTR_ARG_NONNULL(...)                   \
  __attribute__((nonnull(__VA_ARGS__)))

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
