
#include "cplr.h"

int cplr_defaults(cplr_t *c) {
  l_appends(&c->defdef, "-D_DEFAULT_SOURCE");
  l_appends(&c->defdef, "-D_XOPEN_SOURCE=600");
  l_appends(&c->defdef, "-D_POSIX_SOURCE=200809L");
  l_appends(&c->defsys, "sys/stat.h");
  l_appends(&c->defsys, "sys/types.h");
  l_appends(&c->defsys, "assert.h");
  l_appends(&c->defsys, "ctype.h");
  l_appends(&c->defsys, "errno.h");
  l_appends(&c->defsys, "fcntl.h");
  l_appends(&c->defsys, "libgen.h");
  l_appends(&c->defsys, "limits.h");
  l_appends(&c->defsys, "stdarg.h");
  l_appends(&c->defsys, "stdio.h");
  l_appends(&c->defsys, "stdint.h");
  l_appends(&c->defsys, "stdlib.h");
  l_appends(&c->defsys, "string.h");
  l_appends(&c->defsys, "termios.h");
  l_appends(&c->defsys, "time.h");
  l_appends(&c->defsys, "unistd.h");
  return 0;
}
