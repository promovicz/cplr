#ifndef CPLR_STRING_H
#define CPLR_STRING_H

#include <stdarg.h>
#include <string.h>

#include "attr.h"

ATTR_FUN_FORMAT(printf, 2, 0)
ATTR_ARG_FORMAT(2)
extern char *vmsnprintf(size_t limit, const char *fmt, va_list a);

ATTR_FUN_FORMAT(printf, 2, 3)
ATTR_ARG_FORMAT(2)
extern char *msnprintf(size_t limit, const char *fmt, ...);

ATTR_FUN_FORMAT(printf, 1, 0)
ATTR_ARG_FORMAT(1)
extern char *vmsprintf(const char *fmt, va_list a);


ATTR_FUN_FORMAT(printf, 1, 2)
ATTR_ARG_FORMAT(1)
extern char *msprintf(const char *fmt, ...);

#endif /* !CPLR_STRING_H */
