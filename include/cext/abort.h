#ifndef CPLR_ABORT_H
#define CPLR_ABORT_H

#include <stdarg.h>

#include "attr.h"

ATTR_FUN_COLD
ATTR_FUN_NORETURN
extern void xabortm(const char *msg);

ATTR_FUN_COLD
ATTR_FUN_NORETURN
extern void xabortf(const char *fmt, ...);

#endif /* !CPLR_ABORT_H */
