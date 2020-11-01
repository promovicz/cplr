#ifndef LIBS_H
#define LIBS_H

#include <string.h>

#include "libx.h"

char *vmsnprintf(size_t limit, const char *fmt, va_list a) {
  int len, chk;
  char *buf;
  va_list x;
  if(limit > INT_MAX) {
    limit = INT_MAX;
  }
  va_copy(x, a);
  len = vsnprintf(NULL, 0, fmt, x);
  va_end(x);
  if(len > (int)limit) {
    return NULL;
  }
  buf = malloc(len + 1);
  if(!buf) {
    return NULL;
  }
  va_copy(x, a);
  chk = vsnprintf(buf, len + 1, fmt, x);
  va_end(x);
  assert(chk == len);
  fprintf(stderr, "fmt \"%s\" res \"%s\"\n", fmt, buf);
  return buf;
}

char *mmnprintf(size_t limit, const char *fmt, ...) {
  char *res;
  va_list a;
  va_start(a, fmt);
  res = vmsnprintf(limit, fmt, a);
  va_end(a);
  return res;
}

char *vmsprintf(const char *fmt, va_list a) {
  return vmsnprintf(INT_MAX, fmt, a);
}

char *msprintf(const char *fmt, ...) {
  char *res;
  va_list a;
  va_start(a, fmt);
  res = vmsnprintf(INT_MAX, fmt, a);
  va_end(a);
  return res;
}

#endif /* !LIBS_H */
