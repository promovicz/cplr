
#include <stdio.h>
#include <stdlib.h>

#include "cext/abort.h"

ATTR_FUN_COLD
ATTR_FUN_NORETURN
void xabortm(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  fflush(stderr);
  abort();
}

ATTR_FUN_COLD
ATTR_FUN_NORETURN
void xabortf(const char *fmt, ...) {
  va_list a;
  va_start(a, fmt);
  vfprintf(stderr, fmt, a);
  va_end(a);
  fputc('\n', stderr);
  fflush(stderr);
  abort();
}
