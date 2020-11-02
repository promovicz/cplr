
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
void *xcalloc(size_t s, size_t n) {
  X_VOIDPTR_NOTNULL(calloc, "Allocation of %zu objects of size %zu failed", s, n);
}

ATTR_ARG_NONNULL(1)
ATTR_FUN_RETURNS_NONNULL
ATTR_ARG_ALLOC_SIZE(2)
void *xrealloc(void *ptr, size_t s) {
  X_VOIDPTR_NOTNULL(realloc, "Reallocation of %p to size %zu failed", ptr, s);
}

void xfree(void *p) {
  if(!p) {
    xabortm("xfree: Trying to free a NULL pointer");
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
  if(!(p || *p)) {
    xabortm("xptrfree: Trying to free a NULL pointer");
  }
  ptrfree(p);
}

void lptrfree(void **p) {
  if(!p) {
    xabortm("lptrfree: Trying to free a NULL pointer");
  }
  if(*p) {
    free(*p);
    *p = NULL;
  }
}
