#ifndef CPLR_STRING_H
#define CPLR_STRING_H

#include <string.h>

#include "attr.h"

char *vmsnprintf(size_t limit, const char *fmt, va_list a) {
  int len, chk;
  char *buf;
  va_list x;
  // clamp limit
  if(limit > INT_MAX) {
    limit = INT_MAX;
  }
  // determine length
  va_copy(x, a);
  len = vsnprintf(NULL, 0, fmt, x);
  va_end(x);
  if(len > (int)limit) {
    return NULL;
  }
  // allocate
  buf = malloc(len + 1);
  if(!buf) {
    return NULL;
  }
  // format
  va_copy(x, a);
  chk = vsnprintf(buf, len + 1, fmt, x);
  va_end(x);
  // verify
  assert(chk == len);
  // done
  return buf;
}

char *msnprintf(size_t limit, const char *fmt, ...) {
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

#endif /* !CPLR_STRING_H */
